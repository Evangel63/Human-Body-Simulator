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
 * API's and Operators for selecting armature bones in EditMode
 */

/** \file blender/editors/armature/armature_select.c
 *  \ingroup edarmature
 */

#include "DNA_armature_types.h"
#include "DNA_object_types.h"
#include "DNA_scene_types.h"

#include "BLI_blenlib.h"
#include "BLI_math.h"

#include "BKE_context.h"
#include "BKE_deform.h"
#include "BKE_report.h"

#include "BIF_gl.h"

#include "RNA_access.h"
#include "RNA_define.h"

#include "WM_api.h"
#include "WM_types.h"

#include "ED_armature.h"
#include "ED_screen.h"
#include "ED_view3d.h"

#include "armature_intern.h"

/* utility macros fro storing a temp int in the bone (selection flag) */
<<<<<<< HEAD
#define EBONE_PREV_FLAG_GET(ebone) ((void)0, (GET_INT_FROM_POINTER((ebone)->temp)))
#define EBONE_PREV_FLAG_SET(ebone, val) ((ebone)->temp = SET_INT_IN_POINTER(val))
=======
#define EELEMENT_PREV_FLAG_GET(ebone) ((void)0, (GET_INT_FROM_POINTER((ebone)->temp)))
#define EELEMENT_PREV_FLAG_SET(ebone, val) ((ebone)->temp = SET_INT_IN_POINTER(val))
>>>>>>> Initial commit

/* **************** PoseMode & EditMode Selection Buffer Queries *************************** */

/* only for opengl selection indices */
<<<<<<< HEAD
Bone *get_indexed_bone(Object *ob, int index)
=======
ArmatureElement *get_indexed_bone(Object *ob, int index)
>>>>>>> Initial commit
{
	bPoseChannel *pchan;
	if (ob->pose == NULL) return NULL;
	index >>= 16;     // bone selection codes use left 2 bytes
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	pchan = BLI_findlink(&ob->pose->chanbase, index);
	return pchan ? pchan->bone : NULL;
}

/* See if there are any selected bones in this buffer */
/* only bones from base are checked on */
void *get_bone_from_selectbuffer(Scene *scene, Base *base, unsigned int *buffer, short hits, short findunsel, bool do_nearest)
{
	Object *obedit = scene->obedit; // XXX get from context
<<<<<<< HEAD
	Bone *bone;
	EditBone *ebone;
=======
	ArmatureElement *bone;
	EditArmatureElement *ebone;
>>>>>>> Initial commit
	void *firstunSel = NULL, *firstSel = NULL, *data;
	unsigned int hitresult;
	short i;
	bool takeNext = false;
	int minsel = 0xffffffff, minunsel = 0xffffffff;
<<<<<<< HEAD
	
	for (i = 0; i < hits; i++) {
		hitresult = buffer[3 + (i * 4)];
		
		if (!(hitresult & BONESEL_NOSEL)) {
			if (hitresult & BONESEL_ANY) {  /* to avoid including objects in selection */
				bool sel;
				
				hitresult &= ~(BONESEL_ANY);
=======

	for (i = 0; i < hits; i++) {
		hitresult = buffer[3 + (i * 4)];

		if (!(hitresult & ELEMENTSEL_NOSEL)) {
			if (hitresult & ELEMENTSEL_ANY) {  /* to avoid including objects in selection */
				bool sel;

				hitresult &= ~(ELEMENTSEL_ANY);
>>>>>>> Initial commit
				/* Determine what the current bone is */
				if (obedit == NULL || base->object != obedit) {
					/* no singular posemode, so check for correct object */
					if (base->selcol == (hitresult & 0xFFFF)) {
						bone = get_indexed_bone(base->object, hitresult);
<<<<<<< HEAD
						
						if (findunsel)
							sel = (bone->flag & BONE_SELECTED);
						else
							sel = !(bone->flag & BONE_SELECTED);
						
						data = bone;						
=======

						if (findunsel)
							sel = (bone->flag & ELEMENT_SELECTED);
						else
							sel = !(bone->flag & ELEMENT_SELECTED);

						data = bone;
>>>>>>> Initial commit
					}
					else {
						data = NULL;
						sel = 0;
					}
				}
				else {
					bArmature *arm = obedit->data;
<<<<<<< HEAD
					
					ebone = BLI_findlink(arm->edbo, hitresult);
					if (findunsel)
						sel = (ebone->flag & BONE_SELECTED);
					else
						sel = !(ebone->flag & BONE_SELECTED);
					
					data = ebone;
				}
				
=======

					ebone = BLI_findlink(arm->edbo, hitresult);
					if (findunsel)
						sel = (ebone->flag & ELEMENT_SELECTED);
					else
						sel = !(ebone->flag & ELEMENT_SELECTED);

					data = ebone;
				}

>>>>>>> Initial commit
				if (data) {
					if (sel) {
						if (do_nearest) {
							if (minsel > buffer[4 * i + 1]) {
								firstSel = data;
								minsel = buffer[4 * i + 1];
							}
						}
						else {
							if (!firstSel) firstSel = data;
							takeNext = 1;
						}
					}
					else {
						if (do_nearest) {
							if (minunsel > buffer[4 * i + 1]) {
								firstunSel = data;
								minunsel = buffer[4 * i + 1];
							}
						}
						else {
							if (!firstunSel) firstunSel = data;
							if (takeNext) return data;
						}
					}
				}
			}
		}
	}
<<<<<<< HEAD
	
	if (firstunSel)
		return firstunSel;
	else 
=======

	if (firstunSel)
		return firstunSel;
	else
>>>>>>> Initial commit
		return firstSel;
}

/* used by posemode as well editmode */
/* only checks scene->basact! */
/* x and y are mouse coords (area space) */
void *get_nearest_bone(bContext *C, short findunsel, int x, int y)
{
	ViewContext vc;
	rcti rect;
	unsigned int buffer[MAXPICKBUF];
	short hits;
<<<<<<< HEAD
	
	view3d_set_viewcontext(C, &vc);
	
	// rect.xmin = ... mouseco!
	rect.xmin = rect.xmax = x;
	rect.ymin = rect.ymax = y;
	
=======

	view3d_set_viewcontext(C, &vc);

	// rect.xmin = ... mouseco!
	rect.xmin = rect.xmax = x;
	rect.ymin = rect.ymax = y;

>>>>>>> Initial commit
	glInitNames();
	hits = view3d_opengl_select(&vc, buffer, MAXPICKBUF, &rect, true);

	if (hits > 0)
		return get_bone_from_selectbuffer(vc.scene, vc.scene->basact, buffer, hits, findunsel, true);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	return NULL;
}

/* **************** EditMode stuff ********************** */

/* called in space.c */
/* previously "selectconnected_armature" */
static int armature_select_linked_invoke(bContext *C, wmOperator *op, const wmEvent *event)
{
	bArmature *arm;
<<<<<<< HEAD
	EditBone *bone, *curBone, *next;
=======
	EditArmatureElement *bone, *curBone, *next;
>>>>>>> Initial commit
	const bool extend = RNA_boolean_get(op->ptr, "extend");
	Object *obedit = CTX_data_edit_object(C);
	arm = obedit->data;

	view3d_operator_needs_opengl(C);

	if (extend)
		bone = get_nearest_bone(C, 0, event->mval[0], event->mval[1]);
	else
		bone = get_nearest_bone(C, 1, event->mval[0], event->mval[1]);

	if (!bone)
		return OPERATOR_CANCELLED;

	/* Select parents */
	for (curBone = bone; curBone; curBone = next) {
<<<<<<< HEAD
		if ((curBone->flag & BONE_UNSELECTABLE) == 0) {
			if (extend) {
				curBone->flag &= ~(BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
			}
			else {
				curBone->flag |= (BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
			}
		}
		
		if (curBone->flag & BONE_CONNECTED)
=======
		if ((curBone->flag & ELEMENT_UNSELECTABLE) == 0) {
			if (extend) {
				curBone->flag &= ~(ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
			}
			else {
				curBone->flag |= (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
			}
		}

		if (curBone->flag & ELEMENT_CONNECTED)
>>>>>>> Initial commit
			next = curBone->parent;
		else
			next = NULL;
	}

	/* Select children */
	while (bone) {
		for (curBone = arm->edbo->first; curBone; curBone = next) {
			next = curBone->next;
<<<<<<< HEAD
			if ((curBone->parent == bone) && (curBone->flag & BONE_UNSELECTABLE) == 0) {
				if (curBone->flag & BONE_CONNECTED) {
					if (extend)
						curBone->flag &= ~(BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
					else
						curBone->flag |= (BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
=======
			if ((curBone->parent == bone) && (curBone->flag & ELEMENT_UNSELECTABLE) == 0) {
				if (curBone->flag & ELEMENT_CONNECTED) {
					if (extend)
						curBone->flag &= ~(ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
					else
						curBone->flag |= (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
>>>>>>> Initial commit
					bone = curBone;
					break;
				}
				else {
					bone = NULL;
					break;
				}
			}
		}
		if (!curBone)
			bone = NULL;
	}
<<<<<<< HEAD
	
	ED_armature_sync_selection(arm->edbo);
	
	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);
	
=======

	ED_armature_sync_selection(arm->edbo);

	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

>>>>>>> Initial commit
	return OPERATOR_FINISHED;
}

static int armature_select_linked_poll(bContext *C)
{
	return (ED_operator_view3d_active(C) && ED_operator_editarmature(C) );
}

void ARMATURE_OT_select_linked(wmOperatorType *ot)
{
	/* identifiers */
	ot->name = "Select Connected";
	ot->idname = "ARMATURE_OT_select_linked";
	ot->description = "Select bones related to selected ones by parent/child relationships";
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* api callbacks */
	/* leave 'exec' unset */
	ot->invoke = armature_select_linked_invoke;
	ot->poll = armature_select_linked_poll;
<<<<<<< HEAD
	
	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;
	
=======

	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

>>>>>>> Initial commit
	/* properties */
	RNA_def_boolean(ot->srna, "extend", false, "Extend", "Extend selection instead of deselecting everything first");
}

/* does bones and points */
/* note that BONE ROOT only gets drawn for root bones (or without IK) */
<<<<<<< HEAD
static EditBone *get_nearest_editbonepoint(ViewContext *vc, const int mval[2],
                                           ListBase *edbo, int findunsel, int *selmask)
{
	bArmature *arm = (bArmature *)vc->obedit->data;
	EditBone *ebone_next_act = arm->act_edbone;

	EditBone *ebone;
	rcti rect;
	unsigned int buffer[MAXPICKBUF];
	unsigned int hitresult, besthitresult = BONESEL_NOSEL;
=======
static EditArmatureElement *get_nearest_editbonepoint(ViewContext *vc, const int mval[2],
                                           ListBase *edbo, int findunsel, int *selmask)
{
	bArmature *arm = (bArmature *)vc->obedit->data;
	EditArmatureElement *ebone_next_act = arm->act_edelement;

	EditArmatureElement *ebone;
	rcti rect;
	unsigned int buffer[MAXPICKBUF];
	unsigned int hitresult, besthitresult = ELEMENTSEL_NOSEL;
>>>>>>> Initial commit
	int i, mindep = 4;
	short hits;

	glInitNames();
<<<<<<< HEAD
	
	/* find the bone after the current active bone, so as to bump up its chances in selection.
	 * this way overlapping bones will cycle selection state as with objects. */
	if (ebone_next_act &&
	    EBONE_VISIBLE(arm, ebone_next_act) &&
	    ebone_next_act->flag & (BONE_SELECTED | BONE_ROOTSEL | BONE_TIPSEL))
=======

	/* find the bone after the current active bone, so as to bump up its chances in selection.
	 * this way overlapping bones will cycle selection state as with objects. */
	if (ebone_next_act &&
	    EELEMENT_VISIBLE(arm, ebone_next_act) &&
	    ebone_next_act->flag & (ELEMENT_SELECTED | ELEMENT_ROOTSEL | ELEMENT_TIPSEL))
>>>>>>> Initial commit
	{
		ebone_next_act = ebone_next_act->next ? ebone_next_act->next : arm->edbo->first;
	}
	else {
		ebone_next_act = NULL;
	}

	rect.xmin = mval[0] - 5;
	rect.xmax = mval[0] + 5;
	rect.ymin = mval[1] - 5;
	rect.ymax = mval[1] + 5;

	hits = view3d_opengl_select(vc, buffer, MAXPICKBUF, &rect, true);
	if (hits == 0) {
		rect.xmin = mval[0] - 12;
		rect.xmax = mval[0] + 12;
		rect.ymin = mval[1] - 12;
		rect.ymax = mval[1] + 12;
		hits = view3d_opengl_select(vc, buffer, MAXPICKBUF, &rect, true);
	}
	/* See if there are any selected bones in this group */
	if (hits > 0) {
<<<<<<< HEAD
		
		if (hits == 1) {
			if (!(buffer[3] & BONESEL_NOSEL))
=======

		if (hits == 1) {
			if (!(buffer[3] & ELEMENTSEL_NOSEL))
>>>>>>> Initial commit
				besthitresult = buffer[3];
		}
		else {
			for (i = 0; i < hits; i++) {
				hitresult = buffer[3 + (i * 4)];
<<<<<<< HEAD
				if (!(hitresult & BONESEL_NOSEL)) {
					int dep;
					
					ebone = BLI_findlink(edbo, hitresult & ~BONESEL_ANY);
					
					/* clicks on bone points get advantage */
					if (hitresult & (BONESEL_ROOT | BONESEL_TIP)) {
						/* but also the unselected one */
						if (findunsel) {
							if ( (hitresult & BONESEL_ROOT) && (ebone->flag & BONE_ROOTSEL) == 0)
								dep = 1;
							else if ( (hitresult & BONESEL_TIP) && (ebone->flag & BONE_TIPSEL) == 0)
								dep = 1;
							else 
=======
				if (!(hitresult & ELEMENTSEL_NOSEL)) {
					int dep;

					ebone = BLI_findlink(edbo, hitresult & ~ELEMENTSEL_ANY);

					/* clicks on bone points get advantage */
					if (hitresult & (ELEMENTSEL_ROOT | ELEMENTSEL_TIP)) {
						/* but also the unselected one */
						if (findunsel) {
							if ( (hitresult & ELEMENTSEL_ROOT) && (ebone->flag & ELEMENT_ROOTSEL) == 0)
								dep = 1;
							else if ( (hitresult & ELEMENTSEL_TIP) && (ebone->flag & ELEMENT_TIPSEL) == 0)
								dep = 1;
							else
>>>>>>> Initial commit
								dep = 2;
						}
						else {
							dep = 2;
						}
					}
					else {
						/* bone found */
						if (findunsel) {
<<<<<<< HEAD
							if ((ebone->flag & BONE_SELECTED) == 0)
=======
							if ((ebone->flag & ELEMENT_SELECTED) == 0)
>>>>>>> Initial commit
								dep = 2;
							else
								dep = 3;
						}
						else {
							dep = 3;
						}
					}

					if (ebone == ebone_next_act) {
						dep -= 1;
					}

					if (dep < mindep) {
						mindep = dep;
						besthitresult = hitresult;
					}
				}
			}
		}
<<<<<<< HEAD
		
		if (!(besthitresult & BONESEL_NOSEL)) {
			
			ebone = BLI_findlink(edbo, besthitresult & ~BONESEL_ANY);
			
			*selmask = 0;
			if (besthitresult & BONESEL_ROOT)
				*selmask |= BONE_ROOTSEL;
			if (besthitresult & BONESEL_TIP)
				*selmask |= BONE_TIPSEL;
			if (besthitresult & BONESEL_BONE)
				*selmask |= BONE_SELECTED;
=======

		if (!(besthitresult & ELEMENTSEL_NOSEL)) {

			ebone = BLI_findlink(edbo, besthitresult & ~ELEMENTSEL_ANY);

			*selmask = 0;
			if (besthitresult & ELEMENTSEL_ROOT)
				*selmask |= ELEMENT_ROOTSEL;
			if (besthitresult & ELEMENTSEL_TIP)
				*selmask |= ELEMENT_TIPSEL;
			if (besthitresult & ELEMENTSEL_ELEMENT)
				*selmask |= ELEMENT_SELECTED;
>>>>>>> Initial commit
			return ebone;
		}
	}
	*selmask = 0;
	return NULL;
}



/* toggle==0: deselect
 * toggle==1: swap (based on test)
 * toggle==2: swap (no test), CURRENTLY UNUSED
 */
void ED_armature_deselect_all(Object *obedit, int toggle)
{
	bArmature *arm = obedit->data;
<<<<<<< HEAD
	EditBone    *eBone;
	int sel = 1;
	
=======
	EditArmatureElement    *eBone;
//	EditMuscle  *eMuscle;
	int sel = 1;

>>>>>>> Initial commit
	if (toggle == 1) {
		/* Determine if there are any selected bones
		 * and therefore whether we are selecting or deselecting */
		for (eBone = arm->edbo->first; eBone; eBone = eBone->next) {
			//			if (arm->layer & eBone->layer) {
<<<<<<< HEAD
			if (eBone->flag & (BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL)) {
=======
			if (eBone->flag & (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL)) {
>>>>>>> Initial commit
				sel = 0;
				break;
			}
			//			}
		}
<<<<<<< HEAD
=======

//		for (eMuscle = arm->edmu->first; eMuscle; eMuscle = eMuscle->next) {
//            if (eMuscle->flag & (MUSCLE_SELECTED | MUSCLE_TIPSEL | MUSCLE_ROOTSEL)) {
//                sel = 0;
//                break;
//            }
//        }
>>>>>>> Initial commit
	}
	else {
		sel = toggle;
	}

	/*	Set the flags */
	for (eBone = arm->edbo->first; eBone; eBone = eBone->next) {
		if (sel == 2) {
			/* invert selection of bone */
<<<<<<< HEAD
			if (EBONE_VISIBLE(arm, eBone)) {
				eBone->flag ^= (BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
				if (arm->act_edbone == eBone)
					arm->act_edbone = NULL;
=======
			if (EELEMENT_VISIBLE(arm, eBone)) {
				eBone->flag ^= (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
				if (arm->act_edelement == eBone)
					arm->act_edelement = NULL;
>>>>>>> Initial commit
			}
		}
		else if (sel == 1) {
			/* select bone */
<<<<<<< HEAD
			if (EBONE_VISIBLE(arm, eBone)) {
				eBone->flag |= (BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
				if (eBone->parent)
					eBone->parent->flag |= (BONE_TIPSEL);
=======
			if (EELEMENT_VISIBLE(arm, eBone)) {
				eBone->flag |= (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
				if (eBone->parent)
					eBone->parent->flag |= (ELEMENT_TIPSEL);
>>>>>>> Initial commit
			}
		}
		else {
			/* deselect bone */
<<<<<<< HEAD
			eBone->flag &= ~(BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
			if (arm->act_edbone == eBone)
				arm->act_edbone = NULL;
		}
	}
	
=======
			eBone->flag &= ~(ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
			if (arm->act_edelement == eBone)
				arm->act_edelement = NULL;
		}
	}

>>>>>>> Initial commit
	ED_armature_sync_selection(arm->edbo);
}

void ED_armature_deselect_all_visible(Object *obedit)
{
	bArmature *arm = obedit->data;
<<<<<<< HEAD
	EditBone    *ebone;

	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
		/* first and foremost, bone must be visible and selected */
		if (EBONE_VISIBLE(arm, ebone)) {
			ebone->flag &= ~(BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
=======
	EditArmatureElement    *ebone;

	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
		/* first and foremost, bone must be visible and selected */
		if (EELEMENT_VISIBLE(arm, ebone)) {
			ebone->flag &= ~(ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
>>>>>>> Initial commit
		}
	}

	ED_armature_sync_selection(arm->edbo);
}

/* accounts for connected parents */
<<<<<<< HEAD
static int ebone_select_flag(EditBone *ebone)
{
	if (ebone->parent && (ebone->flag & BONE_CONNECTED)) {
		return ((ebone->parent->flag & BONE_TIPSEL) ? BONE_ROOTSEL : 0) | (ebone->flag & (BONE_SELECTED | BONE_TIPSEL));
	}
	else {
		return ebone->flag & (BONE_SELECTED | BONE_ROOTSEL | BONE_TIPSEL);
=======
static int ebone_select_flag(EditArmatureElement *ebone)
{
	if (ebone->parent && (ebone->flag & ELEMENT_CONNECTED)) {
		return ((ebone->parent->flag & ELEMENT_TIPSEL) ? ELEMENT_ROOTSEL : 0) | (ebone->flag & (ELEMENT_SELECTED | ELEMENT_TIPSEL));
	}
	else {
		return ebone->flag & (ELEMENT_SELECTED | ELEMENT_ROOTSEL | ELEMENT_TIPSEL);
>>>>>>> Initial commit
	}
}

/* context: editmode armature in view3d */
bool mouse_armature(bContext *C, const int mval[2], bool extend, bool deselect, bool toggle)
{
	Object *obedit = CTX_data_edit_object(C);
	bArmature *arm = obedit->data;
	ViewContext vc;
<<<<<<< HEAD
	EditBone *nearBone = NULL;
	int selmask;

	view3d_set_viewcontext(C, &vc);
	
	BIF_sk_selectStroke(C, mval, extend);
	
=======
	EditArmatureElement *nearBone = NULL;
	int selmask;

	view3d_set_viewcontext(C, &vc);

	BIF_sk_selectStroke(C, mval, extend);

>>>>>>> Initial commit
	nearBone = get_nearest_editbonepoint(&vc, mval, arm->edbo, 1, &selmask);
	if (nearBone) {

		if (!extend && !deselect && !toggle)
			ED_armature_deselect_all(obedit, 0);
<<<<<<< HEAD
		
		/* by definition the non-root connected bones have no root point drawn,
		 * so a root selection needs to be delivered to the parent tip */
		
		if (selmask & BONE_SELECTED) {
			if (nearBone->parent && (nearBone->flag & BONE_CONNECTED)) {
				/* click in a chain */
				if (extend) {
					/* select this bone */
					nearBone->flag |= BONE_TIPSEL;
					nearBone->parent->flag |= BONE_TIPSEL;
				}
				else if (deselect) {
					/* deselect this bone */
					nearBone->flag &= ~(BONE_TIPSEL | BONE_SELECTED);
					/* only deselect parent tip if it is not selected */
					if (!(nearBone->parent->flag & BONE_SELECTED))
						nearBone->parent->flag &= ~BONE_TIPSEL;
				}
				else if (toggle) {
					/* hold shift inverts this bone's selection */
					if (nearBone->flag & BONE_SELECTED) {
						/* deselect this bone */
						nearBone->flag &= ~(BONE_TIPSEL | BONE_SELECTED);
						/* only deselect parent tip if it is not selected */
						if (!(nearBone->parent->flag & BONE_SELECTED))
							nearBone->parent->flag &= ~BONE_TIPSEL;
					}
					else {
						/* select this bone */
						nearBone->flag |= BONE_TIPSEL;
						nearBone->parent->flag |= BONE_TIPSEL;
=======

		/* by definition the non-root connected bones have no root point drawn,
		 * so a root selection needs to be delivered to the parent tip */

		if (selmask & ELEMENT_SELECTED) {
			if (nearBone->parent && (nearBone->flag & ELEMENT_CONNECTED)) {
				/* click in a chain */
				if (extend) {
					/* select this bone */
					nearBone->flag |= ELEMENT_TIPSEL;
					nearBone->parent->flag |= ELEMENT_TIPSEL;
				}
				else if (deselect) {
					/* deselect this bone */
					nearBone->flag &= ~(ELEMENT_TIPSEL | ELEMENT_SELECTED);
					/* only deselect parent tip if it is not selected */
					if (!(nearBone->parent->flag & ELEMENT_SELECTED))
						nearBone->parent->flag &= ~ELEMENT_TIPSEL;
				}
				else if (toggle) {
					/* hold shift inverts this bone's selection */
					if (nearBone->flag & ELEMENT_SELECTED) {
						/* deselect this bone */
						nearBone->flag &= ~(ELEMENT_TIPSEL | ELEMENT_SELECTED);
						/* only deselect parent tip if it is not selected */
						if (!(nearBone->parent->flag & ELEMENT_SELECTED))
							nearBone->parent->flag &= ~ELEMENT_TIPSEL;
					}
					else {
						/* select this bone */
						nearBone->flag |= ELEMENT_TIPSEL;
						nearBone->parent->flag |= ELEMENT_TIPSEL;
>>>>>>> Initial commit
					}
				}
				else {
					/* select this bone */
<<<<<<< HEAD
					nearBone->flag |= BONE_TIPSEL;
					nearBone->parent->flag |= BONE_TIPSEL;
=======
					nearBone->flag |= ELEMENT_TIPSEL;
					nearBone->parent->flag |= ELEMENT_TIPSEL;
>>>>>>> Initial commit
				}
			}
			else {
				if (extend) {
<<<<<<< HEAD
					nearBone->flag |= (BONE_TIPSEL | BONE_ROOTSEL);
				}
				else if (deselect) {
					nearBone->flag &= ~(BONE_TIPSEL | BONE_ROOTSEL);
				}
				else if (toggle) {
					/* hold shift inverts this bone's selection */
					if (nearBone->flag & BONE_SELECTED)
						nearBone->flag &= ~(BONE_TIPSEL | BONE_ROOTSEL);
					else
						nearBone->flag |= (BONE_TIPSEL | BONE_ROOTSEL);
				}
				else
					nearBone->flag |= (BONE_TIPSEL | BONE_ROOTSEL);
=======
					nearBone->flag |= (ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
				}
				else if (deselect) {
					nearBone->flag &= ~(ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
				}
				else if (toggle) {
					/* hold shift inverts this bone's selection */
					if (nearBone->flag & ELEMENT_SELECTED)
						nearBone->flag &= ~(ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
					else
						nearBone->flag |= (ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
				}
				else
					nearBone->flag |= (ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
>>>>>>> Initial commit
			}
		}
		else {
			if (extend)
				nearBone->flag |= selmask;
			else if (deselect)
				nearBone->flag &= ~selmask;
			else if (toggle && (nearBone->flag & selmask))
				nearBone->flag &= ~selmask;
			else
				nearBone->flag |= selmask;
		}
<<<<<<< HEAD
		
		ED_armature_sync_selection(arm->edbo);
		
		if (nearBone) {
			/* then now check for active status */
			if (ebone_select_flag(nearBone)) {
				arm->act_edbone = nearBone;
			}
		}
		
=======

		ED_armature_sync_selection(arm->edbo);

		if (nearBone) {
			/* then now check for active status */
			if (ebone_select_flag(nearBone)) {
				arm->act_edelement = nearBone;
			}
		}

>>>>>>> Initial commit
		WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, vc.obedit);
		return true;
	}

	return false;
}


/* ****************  Selections  ******************/

static int armature_de_select_all_exec(bContext *C, wmOperator *op)
{
	int action = RNA_enum_get(op->ptr, "action");

	if (action == SEL_TOGGLE) {
		/* Determine if there are any selected bones
		 * And therefore whether we are selecting or deselecting */
		action = SEL_SELECT;
<<<<<<< HEAD
		CTX_DATA_BEGIN(C, EditBone *, ebone, visible_bones)
		{
			if (ebone->flag & (BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL)) {
=======
		CTX_DATA_BEGIN(C, EditArmatureElement *, ebone, visible_bones)
		{
			if (ebone->flag & (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL)) {
>>>>>>> Initial commit
				action = SEL_DESELECT;
				break;
			}
		}
		CTX_DATA_END;
	}
<<<<<<< HEAD
	
	/*	Set the flags */
	CTX_DATA_BEGIN(C, EditBone *, ebone, visible_bones)
=======

	/*	Set the flags */
	CTX_DATA_BEGIN(C, EditArmatureElement *, ebone, visible_bones)
>>>>>>> Initial commit
	{
		/* ignore bone if selection can't change */
		switch (action) {
			case SEL_SELECT:
<<<<<<< HEAD
				if ((ebone->flag & BONE_UNSELECTABLE) == 0) {
					ebone->flag |= (BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
					if (ebone->parent) {
						ebone->parent->flag |= (BONE_TIPSEL);
=======
				if ((ebone->flag & ELEMENT_UNSELECTABLE) == 0) {
					ebone->flag |= (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
					if (ebone->parent) {
						ebone->parent->flag |= (ELEMENT_TIPSEL);
>>>>>>> Initial commit
					}
				}
				break;
			case SEL_DESELECT:
<<<<<<< HEAD
				ebone->flag &= ~(BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
				break;
			case SEL_INVERT:
				if (ebone->flag & BONE_SELECTED) {
					ebone->flag &= ~(BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
				}
				else {
					if ((ebone->flag & BONE_UNSELECTABLE) == 0) {
						ebone->flag |= (BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
						if (ebone->parent) {
							ebone->parent->flag |= (BONE_TIPSEL);
=======
				ebone->flag &= ~(ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
				break;
			case SEL_INVERT:
				if (ebone->flag & ELEMENT_SELECTED) {
					ebone->flag &= ~(ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
				}
				else {
					if ((ebone->flag & ELEMENT_UNSELECTABLE) == 0) {
						ebone->flag |= (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
						if (ebone->parent) {
							ebone->parent->flag |= (ELEMENT_TIPSEL);
>>>>>>> Initial commit
						}
					}
				}
				break;
		}
	}
	CTX_DATA_END;

	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, NULL);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	return OPERATOR_FINISHED;
}

void ARMATURE_OT_select_all(wmOperatorType *ot)
{
	/* identifiers */
	ot->name = "(De)select All";
	ot->idname = "ARMATURE_OT_select_all";
	ot->description = "Toggle selection status of all bones";
<<<<<<< HEAD
	
	/* api callbacks */
	ot->exec = armature_de_select_all_exec;
	ot->poll = ED_operator_editarmature;
	
	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;
	
=======

	/* api callbacks */
	ot->exec = armature_de_select_all_exec;
	ot->poll = ED_operator_editarmature;

	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

>>>>>>> Initial commit
	WM_operator_properties_select_all(ot);
}

/**************** Select more/less **************/

<<<<<<< HEAD
static void armature_select_more(bArmature *arm, EditBone *ebone)
{
	if ((EBONE_PREV_FLAG_GET(ebone) & (BONE_ROOTSEL | BONE_TIPSEL)) != 0) {
		if (EBONE_SELECTABLE(arm, ebone)) {
=======
static void armature_select_more(bArmature *arm, EditArmatureElement *ebone)
{
	if ((EELEMENT_PREV_FLAG_GET(ebone) & (ELEMENT_ROOTSEL | ELEMENT_TIPSEL)) != 0) {
		if (EELEMENT_SELECTABLE(arm, ebone)) {
>>>>>>> Initial commit
			ED_armature_ebone_select_set(ebone, true);
		}
	}

<<<<<<< HEAD
	if (ebone->parent && (ebone->flag & BONE_CONNECTED)) {
		/* to parent */
		if ((EBONE_PREV_FLAG_GET(ebone) & BONE_ROOTSEL) != 0) {
			if (EBONE_SELECTABLE(arm, ebone->parent)) {
				ED_armature_ebone_selectflag_enable(ebone->parent, (BONE_SELECTED | BONE_ROOTSEL | BONE_TIPSEL));
=======
	if (ebone->parent && (ebone->flag & ELEMENT_CONNECTED)) {
		/* to parent */
		if ((EELEMENT_PREV_FLAG_GET(ebone) & ELEMENT_ROOTSEL) != 0) {
			if (EELEMENT_SELECTABLE(arm, ebone->parent)) {
				ED_armature_ebone_selectflag_enable(ebone->parent, (ELEMENT_SELECTED | ELEMENT_ROOTSEL | ELEMENT_TIPSEL));
>>>>>>> Initial commit
			}
		}

		/* from parent (difference from select less) */
<<<<<<< HEAD
		if ((EBONE_PREV_FLAG_GET(ebone->parent) & BONE_TIPSEL) != 0) {
			if (EBONE_SELECTABLE(arm, ebone)) {
				ED_armature_ebone_selectflag_enable(ebone, (BONE_SELECTED | BONE_ROOTSEL));
=======
		if ((EELEMENT_PREV_FLAG_GET(ebone->parent) & ELEMENT_TIPSEL) != 0) {
			if (EELEMENT_SELECTABLE(arm, ebone)) {
				ED_armature_ebone_selectflag_enable(ebone, (ELEMENT_SELECTED | ELEMENT_ROOTSEL));
>>>>>>> Initial commit
			}
		}
	}
}

<<<<<<< HEAD
static void armature_select_less(bArmature *UNUSED(arm), EditBone *ebone)
{
	if ((EBONE_PREV_FLAG_GET(ebone) & (BONE_ROOTSEL | BONE_TIPSEL)) != (BONE_ROOTSEL | BONE_TIPSEL)) {
		ED_armature_ebone_select_set(ebone, false);
	}

	if (ebone->parent && (ebone->flag & BONE_CONNECTED)) {
		/* to parent */
		if ((EBONE_PREV_FLAG_GET(ebone) & BONE_SELECTED) == 0) {
			ED_armature_ebone_selectflag_disable(ebone->parent, (BONE_SELECTED | BONE_TIPSEL));
		}

		/* from parent (difference from select more) */
		if ((EBONE_PREV_FLAG_GET(ebone->parent) & BONE_SELECTED) == 0) {
			ED_armature_ebone_selectflag_disable(ebone, (BONE_SELECTED | BONE_ROOTSEL));
=======
static void armature_select_less(bArmature *UNUSED(arm), EditArmatureElement *ebone)
{
	if ((EELEMENT_PREV_FLAG_GET(ebone) & (ELEMENT_ROOTSEL | ELEMENT_TIPSEL)) != (ELEMENT_ROOTSEL | ELEMENT_TIPSEL)) {
		ED_armature_ebone_select_set(ebone, false);
	}

	if (ebone->parent && (ebone->flag & ELEMENT_CONNECTED)) {
		/* to parent */
		if ((EELEMENT_PREV_FLAG_GET(ebone) & ELEMENT_SELECTED) == 0) {
			ED_armature_ebone_selectflag_disable(ebone->parent, (ELEMENT_SELECTED | ELEMENT_TIPSEL));
		}

		/* from parent (difference from select more) */
		if ((EELEMENT_PREV_FLAG_GET(ebone->parent) & ELEMENT_SELECTED) == 0) {
			ED_armature_ebone_selectflag_disable(ebone, (ELEMENT_SELECTED | ELEMENT_ROOTSEL));
>>>>>>> Initial commit
		}
	}
}

static void armature_select_more_less(Object *ob, bool more)
{
	bArmature *arm = (bArmature *)ob->data;
<<<<<<< HEAD
	EditBone *ebone;
=======
	EditArmatureElement *ebone;
>>>>>>> Initial commit

	/* XXX, eventually we shouldn't need this - campbell */
	ED_armature_sync_selection(arm->edbo);

	/* count bones & store selection state */
	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
<<<<<<< HEAD
		EBONE_PREV_FLAG_SET(ebone, ED_armature_ebone_selectflag_get(ebone));
=======
		EELEMENT_PREV_FLAG_SET(ebone, ED_armature_ebone_selectflag_get(ebone));
>>>>>>> Initial commit
	}

	/* do selection */
	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
<<<<<<< HEAD
		if (EBONE_VISIBLE(arm, ebone)) {
=======
		if (EELEMENT_VISIBLE(arm, ebone)) {
>>>>>>> Initial commit
			if (more) {
				armature_select_more(arm, ebone);
			}
			else {
				armature_select_less(arm, ebone);
			}
		}
	}

	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
<<<<<<< HEAD
		if (EBONE_VISIBLE(arm, ebone)) {
			if (more == false) {
				if (ebone->flag & BONE_SELECTED) {
=======
		if (EELEMENT_VISIBLE(arm, ebone)) {
			if (more == false) {
				if (ebone->flag & ELEMENT_SELECTED) {
>>>>>>> Initial commit
					ED_armature_ebone_select_set(ebone, true);
				}
			}
		}
		ebone->temp = NULL;
	}

	ED_armature_sync_selection(arm->edbo);
}

static int armature_de_select_more_exec(bContext *C, wmOperator *UNUSED(op))
{
	Object *obedit = CTX_data_edit_object(C);
	armature_select_more_less(obedit, true);
	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

	return OPERATOR_FINISHED;
}

void ARMATURE_OT_select_more(wmOperatorType *ot)
{
	/* identifiers */
	ot->name = "Select More";
	ot->idname = "ARMATURE_OT_select_more";
	ot->description = "Select those bones connected to the initial selection";

	/* api callbacks */
	ot->exec = armature_de_select_more_exec;
	ot->poll = ED_operator_editarmature;

	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;
}

static int armature_de_select_less_exec(bContext *C, wmOperator *UNUSED(op))
{
	Object *obedit = CTX_data_edit_object(C);
	armature_select_more_less(obedit, false);
	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

	return OPERATOR_FINISHED;
}

void ARMATURE_OT_select_less(wmOperatorType *ot)
{
	/* identifiers */
	ot->name = "Select Less";
	ot->idname = "ARMATURE_OT_select_less";
	ot->description = "Deselect those bones at the boundary of each selection region";

	/* api callbacks */
	ot->exec = armature_de_select_less_exec;
	ot->poll = ED_operator_editarmature;

	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;
}

enum {
	SIMEDBONE_LENGTH = 1,
	SIMEDBONE_DIRECTION,
	SIMEDBONE_PREFIX,
	SIMEDBONE_SUFFIX,
	SIMEDBONE_LAYER
};

static EnumPropertyItem prop_similar_types[] = {
	{SIMEDBONE_LENGTH, "LENGTH", 0, "Length", ""},
	{SIMEDBONE_DIRECTION, "DIRECTION", 0, "Direction (Y axis)", ""},
	{SIMEDBONE_PREFIX, "PREFIX", 0, "Prefix", ""},
	{SIMEDBONE_SUFFIX, "SUFFIX", 0, "Suffix", ""},
	{SIMEDBONE_LAYER, "LAYER", 0, "Layer", ""},
	{0, NULL, 0, NULL, NULL}
};


<<<<<<< HEAD
static void select_similar_length(bArmature *arm, EditBone *ebone_act, const float thresh)
{
	EditBone *ebone;
=======
static void select_similar_length(bArmature *arm, EditArmatureElement *ebone_act, const float thresh)
{
	EditArmatureElement *ebone;
>>>>>>> Initial commit

	/* thresh is always relative to current length */
	const float len_min = ebone_act->length / (1.0f + thresh);
	const float len_max = ebone_act->length * (1.0f + thresh);

	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
<<<<<<< HEAD
		if (EBONE_SELECTABLE(arm, ebone)) {
=======
		if (EELEMENT_SELECTABLE(arm, ebone)) {
>>>>>>> Initial commit
			if ((ebone->length >= len_min) &&
			    (ebone->length <= len_max))
			{
				ED_armature_ebone_select_set(ebone, true);
			}
		}
	}
}

<<<<<<< HEAD
static void select_similar_direction(bArmature *arm, EditBone *ebone_act, const float thresh)
{
	EditBone *ebone;
=======
static void select_similar_direction(bArmature *arm, EditArmatureElement *ebone_act, const float thresh)
{
	EditArmatureElement *ebone;
>>>>>>> Initial commit
	float dir_act[3];
	sub_v3_v3v3(dir_act, ebone_act->head, ebone_act->tail);

	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
<<<<<<< HEAD
		if (EBONE_SELECTABLE(arm, ebone)) {
=======
		if (EELEMENT_SELECTABLE(arm, ebone)) {
>>>>>>> Initial commit
			float dir[3];
			sub_v3_v3v3(dir, ebone->head, ebone->tail);

			if (angle_v3v3(dir_act, dir) / (float)M_PI < thresh) {
				ED_armature_ebone_select_set(ebone, true);
			}
		}
	}
}

<<<<<<< HEAD
static void select_similar_layer(bArmature *arm, EditBone *ebone_act)
{
	EditBone *ebone;

	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
		if (EBONE_SELECTABLE(arm, ebone)) {
=======
static void select_similar_layer(bArmature *arm, EditArmatureElement *ebone_act)
{
	EditArmatureElement *ebone;

	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
		if (EELEMENT_SELECTABLE(arm, ebone)) {
>>>>>>> Initial commit
			if (ebone->layer & ebone_act->layer) {
				ED_armature_ebone_select_set(ebone, true);
			}
		}
	}
}

<<<<<<< HEAD
static void select_similar_prefix(bArmature *arm, EditBone *ebone_act)
{
	EditBone *ebone;
=======
static void select_similar_prefix(bArmature *arm, EditArmatureElement *ebone_act)
{
	EditArmatureElement *ebone;
>>>>>>> Initial commit

	char body_tmp[MAX_VGROUP_NAME];
	char prefix_act[MAX_VGROUP_NAME];

	BKE_deform_split_prefix(ebone_act->name, prefix_act, body_tmp);

	if (prefix_act[0] == '\0')
		return;

	/* Find matches */
	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
<<<<<<< HEAD
		if (EBONE_SELECTABLE(arm, ebone)) {
=======
		if (EELEMENT_SELECTABLE(arm, ebone)) {
>>>>>>> Initial commit
			char prefix_other[MAX_VGROUP_NAME];
			BKE_deform_split_prefix(ebone->name, prefix_other, body_tmp);
			if (STREQ(prefix_act, prefix_other)) {
				ED_armature_ebone_select_set(ebone, true);
			}
		}
	}
}

<<<<<<< HEAD
static void select_similar_suffix(bArmature *arm, EditBone *ebone_act)
{
	EditBone *ebone;
=======
static void select_similar_suffix(bArmature *arm, EditArmatureElement *ebone_act)
{
	EditArmatureElement *ebone;
>>>>>>> Initial commit

	char body_tmp[MAX_VGROUP_NAME];
	char suffix_act[MAX_VGROUP_NAME];

	BKE_deform_split_suffix(ebone_act->name, body_tmp, suffix_act);

	if (suffix_act[0] == '\0')
		return;

	/* Find matches */
	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
<<<<<<< HEAD
		if (EBONE_SELECTABLE(arm, ebone)) {
=======
		if (EELEMENT_SELECTABLE(arm, ebone)) {
>>>>>>> Initial commit
			char suffix_other[MAX_VGROUP_NAME];
			BKE_deform_split_suffix(ebone->name, body_tmp, suffix_other);
			if (STREQ(suffix_act, suffix_other)) {
				ED_armature_ebone_select_set(ebone, true);
			}
		}
	}
}

static int armature_select_similar_exec(bContext *C, wmOperator *op)
{
	Object *obedit = CTX_data_edit_object(C);
	bArmature *arm = obedit->data;
<<<<<<< HEAD
	EditBone *ebone_act = CTX_data_active_bone(C);
=======
	EditArmatureElement *ebone_act = CTX_data_active_bone(C);
>>>>>>> Initial commit

	/* Get props */
	int type = RNA_enum_get(op->ptr, "type");
	float thresh = RNA_float_get(op->ptr, "threshold");

	/* Check for active bone */
	if (ebone_act == NULL) {
		BKE_report(op->reports, RPT_ERROR, "Operation requires an active bone");
		return OPERATOR_CANCELLED;
	}

	switch (type) {
		case SIMEDBONE_LENGTH:
			select_similar_length(arm, ebone_act, thresh);
			break;
		case SIMEDBONE_DIRECTION:
			select_similar_direction(arm, ebone_act, thresh);
			break;
		case SIMEDBONE_PREFIX:
			select_similar_prefix(arm, ebone_act);
			break;
		case SIMEDBONE_SUFFIX:
			select_similar_suffix(arm, ebone_act);
			break;
		case SIMEDBONE_LAYER:
			select_similar_layer(arm, ebone_act);
			break;
	}

	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

	return OPERATOR_FINISHED;
}

void ARMATURE_OT_select_similar(wmOperatorType *ot)
{
	/* identifiers */
	ot->name = "Select Similar";
	ot->idname = "ARMATURE_OT_select_similar";

	/* callback functions */
	ot->invoke = WM_menu_invoke;
	ot->exec = armature_select_similar_exec;
	ot->poll = ED_operator_editarmature;
	ot->description = "Select similar bones by property types";

	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

	/* properties */
	ot->prop = RNA_def_enum(ot->srna, "type", prop_similar_types, 0, "Type", "");
	RNA_def_float(ot->srna, "threshold", 0.1f, 0.0f, 1.0f, "Threshold", "", 0.0f, 1.0f);
}

/* ********************* select hierarchy operator ************** */

/* Get the first available child of an editbone */
<<<<<<< HEAD
static EditBone *editbone_get_child(bArmature *arm, EditBone *pabone, short use_visibility)
{
	EditBone *curbone, *chbone = NULL;
	
	for (curbone = arm->edbo->first; curbone; curbone = curbone->next) {
		if (curbone->parent == pabone) {
			if (use_visibility) {
				if ((arm->layer & curbone->layer) && !(pabone->flag & BONE_HIDDEN_A)) {
=======
static EditArmatureElement *editbone_get_child(bArmature *arm, EditArmatureElement *pabone, short use_visibility)
{
	EditArmatureElement *curbone, *chbone = NULL;

	for (curbone = arm->edbo->first; curbone; curbone = curbone->next) {
		if (curbone->parent == pabone) {
			if (use_visibility) {
				if ((arm->layer & curbone->layer) && !(pabone->flag & ELEMENT_HIDDEN_A)) {
>>>>>>> Initial commit
					chbone = curbone;
				}
			}
			else
				chbone = curbone;
		}
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	return chbone;
}

static int armature_select_hierarchy_exec(bContext *C, wmOperator *op)
{
	Object *obedit = CTX_data_edit_object(C);
	Object *ob;
	bArmature *arm;
<<<<<<< HEAD
	EditBone *curbone, *pabone, *chbone;
	int direction = RNA_enum_get(op->ptr, "direction");
	const bool add_to_sel = RNA_boolean_get(op->ptr, "extend");
	
	ob = obedit;
	arm = (bArmature *)ob->data;
	
	for (curbone = arm->edbo->first; curbone; curbone = curbone->next) {
		/* only work on bone if it is visible and its selection can change */
		if (EBONE_SELECTABLE(arm, curbone)) {
			if (curbone == arm->act_edbone) {
				if (direction == BONE_SELECT_PARENT) {
					if (curbone->parent == NULL) continue;
					else pabone = curbone->parent;
					
					if (EBONE_VISIBLE(arm, pabone)) {
						pabone->flag |= (BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
						arm->act_edbone = pabone;
						if (pabone->parent) pabone->parent->flag |= BONE_TIPSEL;
						
						if (!add_to_sel) curbone->flag &= ~(BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
						break;
					}
					
=======
	EditArmatureElement *curbone, *pabone, *chbone;
	int direction = RNA_enum_get(op->ptr, "direction");
	const bool add_to_sel = RNA_boolean_get(op->ptr, "extend");

	ob = obedit;
	arm = (bArmature *)ob->data;

	for (curbone = arm->edbo->first; curbone; curbone = curbone->next) {
		/* only work on bone if it is visible and its selection can change */
		if (EELEMENT_SELECTABLE(arm, curbone)) {
			if (curbone == arm->act_edelement) {
				if (direction == ELEMENT_SELECT_PARENT) {
					if (curbone->parent == NULL) continue;
					else pabone = curbone->parent;

					if (EELEMENT_VISIBLE(arm, pabone)) {
						pabone->flag |= (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
						arm->act_edelement = pabone;
						if (pabone->parent) pabone->parent->flag |= ELEMENT_TIPSEL;

						if (!add_to_sel) curbone->flag &= ~(ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
						break;
					}

>>>>>>> Initial commit
				}
				else { // BONE_SELECT_CHILD
					chbone = editbone_get_child(arm, curbone, 1);
					if (chbone == NULL) continue;
<<<<<<< HEAD
					
					if (EBONE_SELECTABLE(arm, chbone)) {
						chbone->flag |= (BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL);
						arm->act_edbone = chbone;
						
						if (!add_to_sel) {
							curbone->flag &= ~(BONE_SELECTED | BONE_ROOTSEL);
							if (curbone->parent) curbone->parent->flag &= ~BONE_TIPSEL;
=======

					if (EELEMENT_SELECTABLE(arm, chbone)) {
						chbone->flag |= (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
						arm->act_edelement = chbone;

						if (!add_to_sel) {
							curbone->flag &= ~(ELEMENT_SELECTED | ELEMENT_ROOTSEL);
							if (curbone->parent) curbone->parent->flag &= ~ELEMENT_TIPSEL;
>>>>>>> Initial commit
						}
						break;
					}
				}
			}
		}
	}
<<<<<<< HEAD
	
	ED_armature_sync_selection(arm->edbo);
	
	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, ob);
	
=======

	ED_armature_sync_selection(arm->edbo);

	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, ob);

>>>>>>> Initial commit
	return OPERATOR_FINISHED;
}

void ARMATURE_OT_select_hierarchy(wmOperatorType *ot)
{
	static EnumPropertyItem direction_items[] = {
<<<<<<< HEAD
		{BONE_SELECT_PARENT, "PARENT", 0, "Select Parent", ""},
		{BONE_SELECT_CHILD, "CHILD", 0, "Select Child", ""},
		{0, NULL, 0, NULL, NULL}
	};
	
=======
		{ELEMENT_SELECT_PARENT, "PARENT", 0, "Select Parent", ""},
		{ELEMENT_SELECT_CHILD, "CHILD", 0, "Select Child", ""},
		{0, NULL, 0, NULL, NULL}
	};

>>>>>>> Initial commit
	/* identifiers */
	ot->name = "Select Hierarchy";
	ot->idname = "ARMATURE_OT_select_hierarchy";
	ot->description = "Select immediate parent/children of selected bones";
<<<<<<< HEAD
	
	/* api callbacks */
	ot->exec = armature_select_hierarchy_exec;
	ot->poll = ED_operator_editarmature;
	
=======

	/* api callbacks */
	ot->exec = armature_select_hierarchy_exec;
	ot->poll = ED_operator_editarmature;

>>>>>>> Initial commit
	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

	/* props */
	RNA_def_enum(ot->srna, "direction", direction_items,
<<<<<<< HEAD
	             BONE_SELECT_PARENT, "Direction", "");
=======
	             ELEMENT_SELECT_PARENT, "Direction", "");
>>>>>>> Initial commit
	RNA_def_boolean(ot->srna, "extend", false, "Extend", "Extend the selection");
}

/****************** Mirror Select ****************/

/**
 * \note clone of #pose_select_mirror_exec keep in sync
 */
static int armature_select_mirror_exec(bContext *C, wmOperator *op)
{
	Object *obedit = CTX_data_edit_object(C);
	bArmature *arm = obedit->data;
<<<<<<< HEAD
	EditBone *ebone, *ebone_mirror_act = NULL;
=======
	EditArmatureElement *ebone, *ebone_mirror_act = NULL;
>>>>>>> Initial commit
	const bool active_only = RNA_boolean_get(op->ptr, "only_active");
	const bool extend = RNA_boolean_get(op->ptr, "extend");

	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
		const int flag = ED_armature_ebone_selectflag_get(ebone);
<<<<<<< HEAD
		EBONE_PREV_FLAG_SET(ebone, flag);
	}

	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
		if (EBONE_SELECTABLE(arm, ebone)) {
			EditBone *ebone_mirror;
			int flag_new = extend ? EBONE_PREV_FLAG_GET(ebone) : 0;

			if ((ebone_mirror = ED_armature_bone_get_mirrored(arm->edbo, ebone)) &&
			    (EBONE_VISIBLE(arm, ebone_mirror)))
			{
				const int flag_mirror = EBONE_PREV_FLAG_GET(ebone_mirror);
				flag_new |= flag_mirror;

				if (ebone == arm->act_edbone) {
=======
		EELEMENT_PREV_FLAG_SET(ebone, flag);
	}

	for (ebone = arm->edbo->first; ebone; ebone = ebone->next) {
		if (EELEMENT_SELECTABLE(arm, ebone)) {
			EditArmatureElement *ebone_mirror;
			int flag_new = extend ? EELEMENT_PREV_FLAG_GET(ebone) : 0;

			if ((ebone_mirror = ED_armature_bone_get_mirrored(arm->edbo, ebone)) &&
			    (EELEMENT_VISIBLE(arm, ebone_mirror)))
			{
				const int flag_mirror = EELEMENT_PREV_FLAG_GET(ebone_mirror);
				flag_new |= flag_mirror;

				if (ebone == arm->act_edelement) {
>>>>>>> Initial commit
					ebone_mirror_act = ebone_mirror;
				}

				/* skip all but the active or its mirror */
<<<<<<< HEAD
				if (active_only && !ELEM(arm->act_edbone, ebone, ebone_mirror)) {
=======
				if (active_only && !ELEM(arm->act_edelement, ebone, ebone_mirror)) {
>>>>>>> Initial commit
					continue;
				}
			}

			ED_armature_ebone_selectflag_set(ebone, flag_new);
		}
	}

	if (ebone_mirror_act) {
<<<<<<< HEAD
		arm->act_edbone = ebone_mirror_act;
=======
		arm->act_edelement = ebone_mirror_act;
>>>>>>> Initial commit
	}

	ED_armature_sync_selection(arm->edbo);

	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

	return OPERATOR_FINISHED;
}

void ARMATURE_OT_select_mirror(wmOperatorType *ot)
{
	/* identifiers */
	ot->name = "Flip Active/Selected Bone";
	ot->idname = "ARMATURE_OT_select_mirror";
	ot->description = "Mirror the bone selection";

	/* api callbacks */
	ot->exec = armature_select_mirror_exec;
	ot->poll = ED_operator_editarmature;

	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

	/* properties */
	RNA_def_boolean(ot->srna, "only_active", false, "Active Only", "Only operate on the active bone");
	RNA_def_boolean(ot->srna, "extend", false, "Extend", "Extend the selection");
}


/****************** Select Path ****************/

<<<<<<< HEAD
static bool armature_shortest_path_select(bArmature *arm, EditBone *ebone_parent, EditBone *ebone_child,
=======
static bool armature_shortest_path_select(bArmature *arm, EditArmatureElement *ebone_parent, EditArmatureElement *ebone_child,
>>>>>>> Initial commit
                                          bool use_parent, bool is_test)
{
	do {

		if (!use_parent && (ebone_child == ebone_parent))
			break;

		if (is_test) {
<<<<<<< HEAD
			if (!EBONE_SELECTABLE(arm, ebone_child)) {
=======
			if (!EELEMENT_SELECTABLE(arm, ebone_child)) {
>>>>>>> Initial commit
				return false;
			}
		}
		else {
<<<<<<< HEAD
			ED_armature_ebone_selectflag_set(ebone_child, (BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL));
=======
			ED_armature_ebone_selectflag_set(ebone_child, (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL));
>>>>>>> Initial commit
		}

		if (ebone_child == ebone_parent)
			break;

		ebone_child = ebone_child->parent;
	} while (true);

	return true;
}

static int armature_shortest_path_pick_invoke(bContext *C, wmOperator *op, const wmEvent *event)
{
	Object *obedit = CTX_data_edit_object(C);
	bArmature *arm = obedit->data;
<<<<<<< HEAD
	EditBone *ebone_src, *ebone_dst;
	EditBone *ebone_isect_parent = NULL;
	EditBone *ebone_isect_child[2];
=======
	EditArmatureElement *ebone_src, *ebone_dst;
	EditArmatureElement *ebone_isect_parent = NULL;
	EditArmatureElement *ebone_isect_child[2];
>>>>>>> Initial commit
	bool changed;

	view3d_operator_needs_opengl(C);

<<<<<<< HEAD
	ebone_src = arm->act_edbone;
=======
	ebone_src = arm->act_edelement;
>>>>>>> Initial commit
	ebone_dst = get_nearest_bone(C, 0, event->mval[0], event->mval[1]);

	/* fallback to object selection */
	if (ELEM(NULL, ebone_src, ebone_dst) || (ebone_src == ebone_dst)) {
		return OPERATOR_PASS_THROUGH;
	}

	ebone_isect_child[0] = ebone_src;
	ebone_isect_child[1] = ebone_dst;


	/* ensure 'ebone_src' is the parent of 'ebone_dst', or set 'ebone_isect_parent' */
	if (ED_armature_ebone_is_child_recursive(ebone_src, ebone_dst)) {
		/* pass */
	}
	else if (ED_armature_ebone_is_child_recursive(ebone_dst, ebone_src)) {
<<<<<<< HEAD
		SWAP(EditBone *, ebone_src, ebone_dst);
=======
		SWAP(EditArmatureElement *, ebone_src, ebone_dst);
>>>>>>> Initial commit
	}
	else if ((ebone_isect_parent = ED_armature_bone_find_shared_parent(ebone_isect_child, 2))) {
		/* pass */
	}
	else {
		/* disconnected bones */
		return OPERATOR_CANCELLED;
	}


	if (ebone_isect_parent) {
		if (armature_shortest_path_select(arm, ebone_isect_parent, ebone_src, false, true) &&
		    armature_shortest_path_select(arm, ebone_isect_parent, ebone_dst, false, true))
		{
			armature_shortest_path_select(arm, ebone_isect_parent, ebone_src, false, false);
			armature_shortest_path_select(arm, ebone_isect_parent, ebone_dst, false, false);
			changed = true;
		}
		else {
			/* unselectable */
			changed = false;
		}
	}
	else {
		if (armature_shortest_path_select(arm, ebone_src, ebone_dst, true, true)) {
			armature_shortest_path_select(arm, ebone_src, ebone_dst, true, false);
			changed = true;
		}
		else {
			/* unselectable */
			changed = false;
		}
	}

	if (changed) {
<<<<<<< HEAD
		arm->act_edbone = ebone_dst;
=======
		arm->act_edelement = ebone_dst;
>>>>>>> Initial commit
		ED_armature_sync_selection(arm->edbo);
		WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

		return OPERATOR_FINISHED;
	}
	else {
		BKE_report(op->reports, RPT_WARNING, "Unselectable bone in chain");
		return OPERATOR_CANCELLED;
	}
}

void ARMATURE_OT_shortest_path_pick(wmOperatorType *ot)
{
	/* identifiers */
	ot->name = "Pick Shortest Path";
	ot->idname = "ARMATURE_OT_shortest_path_pick";
	ot->description = "Select shortest path between two bones";

	/* api callbacks */
	ot->invoke = armature_shortest_path_pick_invoke;
	ot->poll = ED_operator_editarmature;

	/* flags */
	ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;
}