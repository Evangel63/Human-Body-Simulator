/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * Contributor(s): Blender Foundation, 2002-2009 full recode.
 *
 * ***** END GPL LICENSE BLOCK *****
 *
 * Operators and API's for creating bones
 */

/** \file blender/editors/armature/armature_add.c
 *  \ingroup edarmature
 */

#include "DNA_armature_types.h"
#include "DNA_constraint_types.h"
#include "DNA_object_types.h"
#include "DNA_scene_types.h"

#include "MEM_guardedalloc.h"

#include "BLI_blenlib.h"
#include "BLI_math.h"

#include "BKE_action.h"
#include "BKE_constraint.h"
#include "BKE_context.h"
#include "BKE_idprop.h"

#include "RNA_access.h"
#include "RNA_define.h"

#include "WM_api.h"
#include "WM_types.h"

#include "ED_armature.h"
#include "ED_screen.h"
#include "ED_view3d.h"

#include "armature_intern.h"

/* *************** Adding stuff in editmode *************** */

/* default bone add, returns it selected, but without tail set */
/* XXX should be used everywhere, now it mallocs bones still locally in functions */
EditArmatureElement *ED_armature_edit_element_add(bArmature *arm, const char *name, int type)
{
	EditArmatureElement *elem = MEM_callocN(sizeof(EditArmatureElement), "eElement");

	BLI_strncpy(elem->name, name, sizeof(elem->name));
	unique_editarmatureelement_name(arm->edel, elem->name, NULL);

    elem->type = type;
	elem->flag |= ELEMENT_TIPSEL;
	elem->xwidth = 0.1f;
	elem->zwidth = 0.1f;
	elem->rad_head = 0.10f;
	elem->rad_tail = 0.05f;
	elem->segments = 1;
	elem->layer = arm->layer;

    switch(type)
    {
        case AE_BONE:
            BLI_addtail(arm->edel, elem);
            elem->data = MEM_callocN(sizeof(BoneData), "eBoneData");
            ((EditBoneElement*)elem->data)->ease1 = 1.0f;
            ((EditBoneElement*)elem->data)->ease2 = 1.0f;
            ((EditBoneElement*)elem->data)->weight = 1.0f;
            ((EditBoneElement*)elem->data)->dist = 0.25f;
            break;
        case AE_MUSCLE:
            BLI_addtail(arm->edel, elem);
            elem->flag |= ELEMENTSEL_MUSCLE;
            elem->data = MEM_callocN(sizeof(MuscleData), "eMuscleData");
            // TODO:
            // Muscle properties go here
            // None yet
            break;
    }

	return elem;
}

EditArmatureElement *ED_armature_edit_bone_add_primitive(Object *obedit_arm, float length, bool view_aligned)
{
	bArmature *arm = obedit_arm->data;
	EditArmatureElement *elem;

	ED_armature_deselect_all(obedit_arm, 0);

	/* Create an element */
	elem = ED_armature_edit_element_add(arm, "Bone", AE_BONE);

	arm->act_edelement = elem;

	zero_v3(elem->head);
	zero_v3(elem->tail);

	elem->tail[view_aligned ? 1 : 2] = length;

	return elem;
}

EditArmatureElement *ED_armature_edit_muscle_add_primitive(Object *obedit_arm, float length, bool view_aligned)
{
	bArmature *arm = obedit_arm->data;
	EditArmatureElement *elem;

	ED_armature_deselect_all(obedit_arm, 0);

	/* Create an element */
	elem = ED_armature_edit_element_add(arm, "Muscle", AE_MUSCLE);

	arm->act_edelement = elem;

	zero_v3(elem->head);
	zero_v3(elem->tail);

	elem->tail[view_aligned ? 1 : 2] = length;

	return elem;
}


/* previously addvert_armature */
/* the ctrl-click method */
static int armature_click_extrude_exec(bContext *C, wmOperator *UNUSED(op))
{
	View3D *v3d;
	bArmature *arm;
	EditArmatureElement *eelement, *newelement, *flipelement;
	float mat[3][3], imat[3][3];
	const float *curs;
	int a, to_root = 0;
	Object *obedit;
	Scene *scene;

	scene = CTX_data_scene(C);
	v3d = CTX_wm_view3d(C);
	obedit = CTX_data_edit_object(C);
	arm = obedit->data;

	/* find the active or selected bone */
	for (eelement = arm->edel->first; eelement; eelement = eelement->next) {
		if (EELEMENT_VISIBLE(arm, eelement)) {
			if (eelement->flag & ELEMENT_TIPSEL || arm->act_edelement == eelement)
				break;
		}
	}

	if (eelement == NULL) {
		if (eelement == NULL)
			return OPERATOR_CANCELLED;

		to_root = 1;
	}

	ED_armature_deselect_all(obedit, 0);

	/* we re-use code for mirror editing... */
	flipelement = NULL;
	if (arm->flag & ARM_MIRROR_EDIT)
		flipelement = ED_armature_element_get_mirrored(arm->edel, eelement);

	for (a = 0; a < 2; a++) {
		if (a == 1) {
			if (flipelement == NULL)
				break;
			else {
				SWAP(EditArmatureElement *, flipelement, eelement);
			}
		}

		newelement = ED_armature_edit_element_add(arm, eelement->name, eelement->type);
		arm->act_edelement = newelement;

		if (to_root) {
			copy_v3_v3(newelement->head, eelement->head);
			newelement->rad_head = eelement->rad_tail;
			newelement->parent = eelement->parent;
		}
		else {
			copy_v3_v3(newelement->head, eelement->tail);
			newelement->rad_head = eelement->rad_tail;
			newelement->parent = eelement;
			newelement->flag |= ELEMENT_CONNECTED;
		}

		curs = ED_view3d_cursor3d_get(scene, v3d);
		copy_v3_v3(newelement->tail, curs);
		sub_v3_v3v3(newelement->tail, newelement->tail, obedit->obmat[3]);

		if (a == 1)
			newelement->tail[0] = -newelement->tail[0];

		copy_m3_m4(mat, obedit->obmat);
		invert_m3_m3(imat, mat);
		mul_m3_v3(imat, newelement->tail);

		newelement->length = len_v3v3(newelement->head, newelement->tail);
		newelement->rad_tail = newelement->length * 0.05f;
		((BoneData*)newelement->data)->dist = newelement->length * 0.25f;

	}

	ED_armature_sync_selection(arm->edel);

	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

	return OPERATOR_FINISHED;
}

static int armature_click_extrude_invoke(bContext *C, wmOperator *op, const wmEvent *event)
{
	/* TODO most of this code is copied from set3dcursor_invoke,
	 * it would be better to reuse code in set3dcursor_invoke */

	/* temporarily change 3d cursor position */
	Scene *scene;
	ARegion *ar;
	View3D *v3d;
	float *fp, tvec[3], oldcurs[3], mval_f[2];
	int retv;

	scene = CTX_data_scene(C);
	ar = CTX_wm_region(C);
	v3d = CTX_wm_view3d(C);

	fp = ED_view3d_cursor3d_get(scene, v3d);

	copy_v3_v3(oldcurs, fp);

	VECCOPY2D(mval_f, event->mval);
	ED_view3d_win_to_3d(ar, fp, mval_f, tvec);
	copy_v3_v3(fp, tvec);

	/* extrude to the where new cursor is and store the operation result */
	retv = armature_click_extrude_exec(C, op);

	/* restore previous 3d cursor position */
	copy_v3_v3(fp, oldcurs);

	return retv;
}

void ARMATURE_OT_click_extrude(wmOperatorType *ot)
{
	/* identifiers */
	ot->name = "Click-Extrude";
	ot->idname = "ARMATURE_OT_click_extrude";
	ot->description = "Create a new bone going from the last selected joint to the mouse position";

	/* api callbacks */
	ot->invoke = armature_click_extrude_invoke;
	ot->exec = armature_click_extrude_exec;
	ot->poll = ED_operator_editarmature;

	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

	/* props */
}

/* adds an EditBone between the nominated locations (should be in the right space) */
EditArmatureElement *add_points_bone(Object *obedit, float head[3], float tail[3])
{
	EditArmatureElement *eel;

	eel = ED_armature_edit_element_add(obedit->data, "Bone", AE_BONE);

	copy_v3_v3(eel->head, head);
	copy_v3_v3(eel->tail, tail);

	return eel;
}


static EditArmatureElement *get_named_editarmatureelement(ListBase *edbo, const char *name)
{
	EditArmatureElement  *eElem;

	if (name) {
		for (eElem = edbo->first; eElem; eElem = eElem->next) {
			if (!strcmp(name, eElem->name))
				return eElem;
		}
	}

	return NULL;
}

/* Call this before doing any duplications
 * */
void preEditArmatureElementDuplicate(ListBase *editbones)
{
	EditArmatureElement *eElem;

	/* clear temp */
	for (eElem = editbones->first; eElem; eElem = eElem->next) {
		eElem->temp = NULL;
	}
}

/*
 * Note: When duplicating cross objects, editbones here is the list of bones
 * from the SOURCE object but ob is the DESTINATION object
 * */
void updateDuplicateSubtargetObjects(EditArmatureElement *dupBone, ListBase *editbones, Object *src_ob, Object *dst_ob)
{
	/* If an edit bone has been duplicated, lets
	 * update it's constraints if the subtarget
	 * they point to has also been duplicated
	 */
	EditArmatureElement     *oldtarget, *newtarget;
	bPoseChannel *pchan;
	bConstraint  *curcon;
	ListBase     *conlist;

	if ((pchan = BKE_pose_channel_verify(dst_ob->pose, dupBone->name))) {
		if ((conlist = &pchan->constraints)) {
			for (curcon = conlist->first; curcon; curcon = curcon->next) {
				/* does this constraint have a subtarget in
				 * this armature?
				 */
				bConstraintTypeInfo *cti = BKE_constraint_typeinfo_get(curcon);
				ListBase targets = {NULL, NULL};
				bConstraintTarget *ct;

				if (cti && cti->get_constraint_targets) {
					cti->get_constraint_targets(curcon, &targets);

					for (ct = targets.first; ct; ct = ct->next) {
						if ((ct->tar == src_ob) && (ct->subtarget[0])) {
							ct->tar = dst_ob; /* update target */
							oldtarget = get_named_editarmatureelement(editbones, ct->subtarget);
							if (oldtarget) {
								/* was the subtarget bone duplicated too? If
								 * so, update the constraint to point at the
								 * duplicate of the old subtarget.
								 */
								if (oldtarget->temp) {
									newtarget = (EditArmatureElement *) oldtarget->temp;
									BLI_strncpy(ct->subtarget, newtarget->name, sizeof(ct->subtarget));
								}
							}
						}
					}

					if (cti->flush_constraint_targets)
						cti->flush_constraint_targets(curcon, &targets, 0);
				}
			}
		}
	}
}

void updateDuplicateSubtarget(EditArmatureElement *dupBone, ListBase *editbones, Object *ob)
{
	updateDuplicateSubtargetObjects(dupBone, editbones, ob, ob);
}

EditArmatureElement *duplicateEditArmatureElementObjects(EditArmatureElement *curBone, const char *name, ListBase *editbones,
                                   Object *src_ob, Object *dst_ob)
{
	EditArmatureElement *eElem = MEM_mallocN(sizeof(EditArmatureElement), "addup_editbone");

	/*	Copy data from old bone to new bone */
	memcpy(eElem, curBone, sizeof(EditArmatureElement));

	switch(curBone->type)
	{
	case AE_BONE:
		eElem->data = MEM_mallocN(sizeof(EditBoneElement), "addup_editbonedata");
		memcpy(eElem->data, curBone->data, sizeof(EditBoneElement));
		break;
	case AE_MUSCLE:
		eElem->data = MEM_mallocN(sizeof(EditMuscleElement), "addup_editmuscledata");
		memcpy(eElem->data, curBone->data, sizeof(EditMuscleElement));
		break;
	}

	curBone->temp = eElem;
	eElem->temp = curBone;

	if (name != NULL) {
		BLI_strncpy(eElem->name, name, sizeof(eElem->name));
	}

	unique_editarmatureelement_name(editbones, eElem->name, NULL);
	BLI_addtail(editbones, eElem);

	/* copy the ID property */
	if (curBone->prop)
		eElem->prop = IDP_CopyProperty(curBone->prop);

	/* Lets duplicate the list of constraints that the
	 * current bone has.
	 */
	if (src_ob->pose) {
		bPoseChannel *chanold, *channew;

		chanold = BKE_pose_channel_verify(src_ob->pose, curBone->name);
		if (chanold) {
			/* WARNING: this creates a new posechannel, but there will not be an attached bone
			 *		yet as the new bones created here are still 'EditBones' not 'Bones'.
			 */
			channew = BKE_pose_channel_verify(dst_ob->pose, eElem->name);

			if (channew) {
				BKE_pose_channel_copy_data(channew, chanold);
			}
		}
	}

	return eElem;
}

EditArmatureElement *duplicateEditBone(EditArmatureElement *curElem, const char *name, ListBase *editbones, Object *ob)
{
	return duplicateEditArmatureElementObjects(curElem, name, editbones, ob, ob);
}

/* previously adduplicate_armature */
static int armature_duplicate_selected_exec(bContext *C, wmOperator *UNUSED(op))
{
	bArmature *arm;
	EditArmatureElement    *eElem = NULL;
	EditArmatureElement    *curElem;
	EditArmatureElement    *firstDup = NULL; /*	The beginning of the duplicated bones in the edbo list */

	Object *obedit = CTX_data_edit_object(C);

    arm = obedit->data;

	/* cancel if nothing selected */
	if (CTX_DATA_COUNT(C, selected_bones) == 0)
		return OPERATOR_CANCELLED;

    ED_armature_sync_selection(arm->edel); // XXX why is this needed?

    preEditArmatureElementDuplicate(arm->edel);

    /* Select mirrored bones */
    if (arm->flag & ARM_MIRROR_EDIT) {
        for (curElem = arm->edel->first; curElem; curElem = curElem->next) {
            if (EELEMENT_VISIBLE(arm, curElem)) {
                if (curElem->flag & ELEMENT_SELECTED) {
                    eElem = ED_armature_element_get_mirrored(arm->edel, curElem);
                    if (eElem)
                        eElem->flag |= ELEMENT_SELECTED;
                }
            }
        }
    }

    /*	Find the selected bones and duplicate them as needed */
    for (curElem = arm->edel->first; curElem && curElem != firstDup; curElem = curElem->next) {
        if (EELEMENT_VISIBLE(arm, curElem)) {
            if (curElem->flag & ELEMENT_SELECTED) {

                eElem = duplicateEditBone(curElem, curElem->name, arm->edel, obedit);

                if (!firstDup)
                    firstDup = eElem;

            }
        }
    }

    /*	Run though the list and fix the pointers */
    for (curElem = arm->edel->first; curElem && curElem != firstDup; curElem = curElem->next) {
        if (EELEMENT_VISIBLE(arm, curElem)) {
            if (curElem->flag & ELEMENT_SELECTED) {
                eElem = (EditArmatureElement *) curElem->temp;

                if (!curElem->parent) {
                    /* If this bone has no parent,
                     * Set the duplicate->parent to NULL
                     */
                    eElem->parent = NULL;
                }
                else if (curElem->parent->temp) {
                    /* If this bone has a parent that was duplicated,
                     * Set the duplicate->parent to the curElem->parent->temp
                     */
                    eElem->parent = (EditArmatureElement *)curElem->parent->temp;
                }
                else {
                    /* If this bone has a parent that IS not selected,
                     * Set the duplicate->parent to the curElem->parent
                     */
                    eElem->parent = (EditArmatureElement *) curElem->parent;
                    eElem->flag &= ~ELEMENT_CONNECTED;
                }

                /* Lets try to fix any constraint subtargets that might
                 * have been duplicated
                 */
                updateDuplicateSubtarget(eElem, arm->edel, obedit);
            }
        }
    }

    /* correct the active bone */
    if (arm->act_edelement) {
        eElem = arm->act_edelement;
        if (eElem->temp)
            arm->act_edelement = eElem->temp;
    }

    /*	Deselect the old bones and select the new ones */
    for (curElem = arm->edel->first; curElem && curElem != firstDup; curElem = curElem->next) {
        if (EELEMENT_VISIBLE(arm, curElem))
            curElem->flag &= ~(ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
    }

	ED_armature_validate_active(arm);

	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

	return OPERATOR_FINISHED;
}


void ARMATURE_OT_duplicate(wmOperatorType *ot)
{
	/* identifiers */
	ot->name = "Duplicate Selected Bone(s)";
	ot->idname = "ARMATURE_OT_duplicate";
	ot->description = "Make copies of the selected bones within the same armature";

	/* api callbacks */
	ot->exec = armature_duplicate_selected_exec;
	ot->poll = ED_operator_editarmature;

	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;
}

/* ------------------------------------------ */

/* previously extrude_armature */
/* context; editmode armature */
/* if forked && mirror-edit: makes two bones with flipped names */
static int armature_extrude_exec(bContext *C, wmOperator *op)
{
	Object *obedit;
	bArmature *arm;
	EditArmatureElement *newelem, *ebone, *flipbone, *first = NULL;
	int a, totbone = 0, do_extrude;
	bool forked = RNA_boolean_get(op->ptr, "forked");

    obedit = CTX_data_edit_object(C);
	arm = obedit->data;

    first = NULL;
    /* since we allow root extrude too, we have to make sure selection is OK */
    for (ebone = arm->edel->first; ebone; ebone = ebone->next) {
        if (EELEMENT_VISIBLE(arm, ebone)) {
            if (ebone->flag & ELEMENT_ROOTSEL) {
                if (ebone->parent && (ebone->flag & ELEMENT_CONNECTED)) {
                    if (ebone->parent->flag & ELEMENT_TIPSEL)
                        ebone->flag &= ~ELEMENT_ROOTSEL;
                }
            }
        }
    }

    /* Duplicate the necessary bones */
    for (ebone = arm->edel->first; ((ebone) && (ebone != first)); ebone = ebone->next) {
        if (EELEMENT_VISIBLE(arm, ebone)) {
            /* we extrude per definition the tip */
            do_extrude = false;
            if (ebone->flag & (ELEMENT_TIPSEL)) { //| ELEMENT_CONNECTED)) {
                do_extrude = true;
            }
            else if (ebone->flag & ELEMENT_ROOTSEL) {
                /* but, a bone with parent deselected we do the root... */
                if (ebone->parent && (ebone->parent->flag & ELEMENT_TIPSEL)) {
                    /* pass */
                }
                else {
                    do_extrude = 2;
                }
            }

            if (do_extrude) {
                /* we re-use code for mirror editing... */
                flipbone = NULL;
                if (arm->flag & ARM_MIRROR_EDIT) {
                    flipbone = ED_armature_element_get_mirrored(arm->edel, ebone);
                    if (flipbone) {
                        forked = 0;  // we extrude 2 different bones
                        if (flipbone->flag & (ELEMENT_TIPSEL | ELEMENT_ROOTSEL | ELEMENT_CONNECTED))
                            /* don't want this bone to be selected... */
                            flipbone->flag &= ~(ELEMENT_TIPSEL | ELEMENT_CONNECTED | ELEMENT_ROOTSEL);
                    }
                    if ((flipbone == NULL) && (forked))
                        flipbone = ebone;
                }

                for (a = 0; a < 2; a++) {
                    if (a == 1) {
                        if (flipbone == NULL)
                            break;
                        else {
                            SWAP(EditArmatureElement *, flipbone, ebone);
                        }
                    }

                    totbone++;
                    newelem = MEM_callocN(sizeof(EditArmatureElement), "extrudebone");

                    if (do_extrude == true) {
                        copy_v3_v3(newelem->head, ebone->tail);
                        copy_v3_v3(newelem->tail, newelem->head);
                        newelem->parent = ebone;

                        newelem->flag = ebone->flag & (ELEMENT_TIPSEL | ELEMENT_RELATIVE_PARENTING);  // copies it, in case mirrored bone

                        if (newelem->parent) newelem->flag |= ELEMENT_CONNECTED;
                    }
                    else {
                        copy_v3_v3(newelem->head, ebone->head);
                        copy_v3_v3(newelem->tail, ebone->head);
                        newelem->parent = ebone->parent;

                        newelem->flag = ELEMENT_TIPSEL;

                        if (newelem->parent && (ebone->flag & ELEMENT_CONNECTED)) {
                            newelem->flag |= ELEMENT_CONNECTED;
                        }
                    }

                    newelem->xwidth = ebone->xwidth;
                    newelem->zwidth = ebone->zwidth;
                    newelem->rad_head = ebone->rad_tail; // don't copy entire bone...
                    newelem->rad_tail = ebone->rad_tail;
                    newelem->segments = 1;
                    newelem->layer = ebone->layer;
                    newelem->type = ebone->type;

                    switch(newelem->type) {
                    case(AE_BONE):
                        newelem->data = MEM_callocN(sizeof(EditBoneElement), "extrudebonedata");
                        ((EditBoneElement*)newelem->data)->weight = ((EditBoneElement*)ebone->data)->weight;
                        ((EditBoneElement*)newelem->data)->dist = ((EditBoneElement*)ebone->data)->dist;
                        ((EditBoneElement*)newelem->data)->ease1 = ((EditBoneElement*)ebone->data)->ease1;
                        ((EditBoneElement*)newelem->data)->ease2 = ((EditBoneElement*)ebone->data)->ease2;
                        break;
                    case(AE_MUSCLE):
                        newelem->data = MEM_callocN(sizeof(EditMuscleElement), "extrudemuscledata");
                        break;
                    }

                    BLI_strncpy(newelem->name, ebone->name, sizeof(newelem->name));

                    if (flipbone && forked) {   // only set if mirror edit
                        if (strlen(newelem->name) < (MAXBONENAME - 2)) {
                            if (a == 0) strcat(newelem->name, "_L");
                            else strcat(newelem->name, "_R");
                        }
                    }
                    unique_editarmatureelement_name(arm->edel, newelem->name, NULL);

                    /* Add the new bone to the list */
                    BLI_addtail(arm->edel, newelem);
                    if (!first)
                        first = newelem;

                    /* restore ebone if we were flipping */
                    if (a == 1 && flipbone)
                        SWAP(EditArmatureElement *, flipbone, ebone);
                }
            }

            /* Deselect the old bone */
            ebone->flag &= ~(ELEMENT_TIPSEL | ELEMENT_SELECTED | ELEMENT_ROOTSEL);
        }
    }
    /* if only one bone, make this one active */
    if (totbone == 1 && first) arm->act_edelement = first;

    if (totbone == 0) return OPERATOR_CANCELLED;

    /* Transform the endpoints */
    ED_armature_sync_selection(arm->edel);

	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

	return OPERATOR_FINISHED;
}

void ARMATURE_OT_extrude(wmOperatorType *ot)
{
	/* identifiers */
	ot->name = "Extrude";
	ot->idname = "ARMATURE_OT_extrude";
	ot->description = "Create new bones from the selected joints";

	/* api callbacks */
	ot->exec = armature_extrude_exec;
	ot->poll = ED_operator_editarmature;

	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

	/* props */
	RNA_def_boolean(ot->srna, "forked", 0, "Forked", "");
}

/* ********************** Bone Add *************************************/

/* Helper function that actually makes bones or muscles */
static int armature_element_primitive_add_exec(bContext *C, wmOperator *op, int element_type)
{
    RegionView3D *rv3d = CTX_wm_region_view3d(C);
	Object *obedit = CTX_data_edit_object(C);
	EditArmatureElement *bone;
	float obmat[3][3], curs[3], viewmat[3][3], totmat[3][3], imat[3][3];
	char name[MAXBONENAME];

	RNA_string_get(op->ptr, "name", name);

	copy_v3_v3(curs, ED_view3d_cursor3d_get(CTX_data_scene(C), CTX_wm_view3d(C)));

	/* Get inverse point for head and orientation for tail */
	invert_m4_m4(obedit->imat, obedit->obmat);
	mul_m4_v3(obedit->imat, curs);

	if (rv3d && (U.flag & USER_ADD_VIEWALIGNED))
		copy_m3_m4(obmat, rv3d->viewmat);
	else unit_m3(obmat);

	copy_m3_m4(viewmat, obedit->obmat);
	mul_m3_m3m3(totmat, obmat, viewmat);
	invert_m3_m3(imat, totmat);

	ED_armature_deselect_all(obedit, 0);

	/*	Create a bone	*/
	bone = ED_armature_edit_element_add(obedit->data, name, element_type);

	copy_v3_v3(bone->head, curs);

	if (rv3d && (U.flag & USER_ADD_VIEWALIGNED))
		add_v3_v3v3(bone->tail, bone->head, imat[1]);   // bone with unit length 1
	else
		add_v3_v3v3(bone->tail, bone->head, imat[2]);   // bone with unit length 1, pointing up Z

	/* note, notifier might evolve */
	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

	return OPERATOR_FINISHED;
}

/*op makes a new bone and returns it with its tip selected */
static int armature_bone_primitive_add_exec(bContext *C, wmOperator *op)
{
    return armature_element_primitive_add_exec(C, op, AE_BONE);
}

void ARMATURE_OT_bone_primitive_add(wmOperatorType *ot)
{
	/* identifiers */
	ot->name = "Add Bone";
	ot->idname = "ARMATURE_OT_bone_primitive_add";
	ot->description = "Add a new bone located at the 3D-Cursor";

	/* api callbacks */
	ot->exec = armature_bone_primitive_add_exec;
	ot->poll = ED_operator_editarmature;

	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

	RNA_def_string(ot->srna, "name", "Bone", MAXBONENAME, "Name", "Name of the newly created bone");
}

/* Mostly a copy of armature_bone_primitive_add_exec except for one line, can it be done better? */
static int armature_muscle_primitive_add_exec(bContext *C, wmOperator *op)
{
    return armature_element_primitive_add_exec(C, op, AE_MUSCLE);
}

void ARMATURE_OT_muscle_primitive_add(wmOperatorType *ot)
{
    /* identifiers */
    ot->name = "Add Muscle";
    ot->idname = "ARMATURE_OT_muscle_primitive_add";
    ot->description = "Add a new muscle located at the 3D-Cursor";

    /* api callbacks */
    ot->exec = armature_muscle_primitive_add_exec;
    ot->poll = ED_operator_editarmature;

    /* flags */
    ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

    RNA_def_string(ot->srna, "name", "Muscle", MAXMUSCLENAME, "Name", "Name of the newly created muscle");
}

/* ********************** Subdivide *******************************/

/* Subdivide Operators:
 * This group of operators all use the same 'exec' callback, but they are called
 * through several different operators - a combined menu (which just calls the exec in the
 * appropriate ways), and two separate ones.
 */

static int armature_subdivide_exec(bContext *C, wmOperator *op)
{
	Object *obedit = CTX_data_edit_object(C);
	bArmature *arm = obedit->data;
	EditArmatureElement *newbone, *tbone;
	int cuts, i;

	/* there may not be a number_cuts property defined (for 'simple' subdivide) */
	cuts = RNA_int_get(op->ptr, "number_cuts");

    /* loop over all editable bones */
    // XXX the old code did this in reverse order though!
    CTX_DATA_BEGIN(C, EditArmatureElement *, ebone, selected_editable_bones)
    {
        for (i = cuts + 1; i > 1; i--) {
            /* compute cut ratio first */
            float cutratio = 1.0f / (float)i;
            float cutratioI = 1.0f - cutratio;

            float val1[3];
            float val2[3];
            float val3[3];

            newbone = MEM_mallocN(sizeof(EditArmatureElement), "ebone subdiv");
            *newbone = *ebone;
            newbone->data = ebone->data;
            BLI_addtail(arm->edel, newbone);

            /* calculate location of newbone->head */
            copy_v3_v3(val1, ebone->head);
            copy_v3_v3(val2, ebone->tail);
            copy_v3_v3(val3, newbone->head);

            val3[0] = val1[0] * cutratio + val2[0] * cutratioI;
            val3[1] = val1[1] * cutratio + val2[1] * cutratioI;
            val3[2] = val1[2] * cutratio + val2[2] * cutratioI;

            copy_v3_v3(newbone->head, val3);
            copy_v3_v3(newbone->tail, ebone->tail);
            copy_v3_v3(ebone->tail, newbone->head);

            newbone->rad_head = ((ebone->rad_head * cutratio) + (ebone->rad_tail * cutratioI));
            ebone->rad_tail = newbone->rad_head;

            newbone->flag |= ELEMENT_CONNECTED;

            newbone->prop = NULL;

            unique_editarmatureelement_name(arm->edel, newbone->name, NULL);

            /* correct parent bones */
            for (tbone = arm->edel->first; tbone; tbone = tbone->next) {
                if (tbone->parent == ebone)
                    tbone->parent = newbone;
            }
            newbone->parent = ebone;
        }
    }
    CTX_DATA_END;

	/* note, notifier might evolve */
	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

	return OPERATOR_FINISHED;
}

void ARMATURE_OT_subdivide(wmOperatorType *ot)
{
	PropertyRNA *prop;

	/* identifiers */
	ot->name = "Subdivide Multi";
	ot->idname = "ARMATURE_OT_subdivide";
	ot->description = "Break selected bones into chains of smaller bones";

	/* api callbacks */
	ot->exec = armature_subdivide_exec;
	ot->poll = ED_operator_editarmature;

	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

	/* Properties */
	prop = RNA_def_int(ot->srna, "number_cuts", 1, 1, INT_MAX, "Number of Cuts", "", 1, 10);
	/* avoid re-using last var because it can cause _very_ high poly meshes and annoy users (or worse crash) */
	RNA_def_property_flag(prop, PROP_SKIP_SAVE);
}
