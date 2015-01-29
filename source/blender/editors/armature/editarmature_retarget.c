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
 * Contributor(s): Martin Poirier
 *
 * ***** END GPL LICENSE BLOCK *****
 * autoarmature.c: Interface for automagically manipulating armature (retarget, created, ...)
 */

/** \file blender/editors/armature/editarmature_retarget.c
 *  \ingroup edarmature
 */

#include "MEM_guardedalloc.h"

#include "PIL_time.h"

#include "DNA_armature_types.h"
#include "DNA_constraint_types.h"
#include "DNA_scene_types.h"
#include "DNA_object_types.h"

#include "BLI_blenlib.h"
#include "BLI_math.h"

#include "BKE_constraint.h"
#include "BKE_armature.h"
#include "BKE_context.h"

#include "ED_armature.h"
#include "ED_util.h"

#include "BIF_retarget.h"

#include "armature_intern.h"

/************ RIG RETARGET DATA STRUCTURES ***************/

typedef struct MemoNode {
	float weight;
	int next;
} MemoNode;

typedef struct RetargetParam {
	RigGraph    *rigg;
	RigArc      *iarc;
	RigNode     *inode_start;
	bContext    *context;
} RetargetParam;

typedef enum  {
	RETARGET_LENGTH,
	RETARGET_AGGRESSIVE
<<<<<<< HEAD
} RetargetMode; 
=======
} RetargetMode;
>>>>>>> Initial commit

typedef enum {
	METHOD_BRUTE_FORCE = 0,
	METHOD_MEMOIZE = 1
} RetargetMethod;

typedef enum {
	ARC_FREE = 0,
	ARC_TAKEN = 1,
	ARC_USED = 2
} ArcUsageFlags;

static RigGraph *GLOBAL_RIGG = NULL;

/*******************************************************************************************************/

void exec_retargetArctoArc(TaskPool *pool, void *taskdata, int threadid);

static void RIG_calculateEdgeAngles(RigEdge *edge_first, RigEdge *edge_second);
<<<<<<< HEAD
float rollBoneByQuat(EditBone *bone, float old_up_axis[3], float qrot[4]);

/* two levels */
#define SHAPE_LEVELS (SHAPE_RADIX * SHAPE_RADIX) 

/*********************************** EDITBONE UTILS ****************************************************/

static int countEditBoneChildren(ListBase *list, EditBone *parent)
{
	EditBone *ebone;
	int count = 0;
	
	for (ebone = list->first; ebone; ebone = ebone->next) {
		if (ebone->parent == parent) {
			count++;
		}
	}
	
	return count;
}

static EditBone *nextEditBoneChild(ListBase *list, EditBone *parent, int n)
{
	EditBone *ebone;
	
	for (ebone = list->first; ebone; ebone = ebone->next) {
		if (ebone->parent == parent) {
			if (n == 0) {
				return ebone;
=======
float rollBoneByQuat(EditArmatureElement *bone, float old_up_axis[3], float qrot[4]);

/* two levels */
#define SHAPE_LEVELS (SHAPE_RADIX * SHAPE_RADIX)

/*********************************** EDITBONE UTILS ****************************************************/

static int countEditArmatureElementChildren(ListBase *list, EditArmatureElement *parent)
{
	EditArmatureElement *eelement;
	int count = 0;

	for (eelement = list->first; eelement; eelement = eelement->next) {
		if (eelement->parent == parent) {
			count++;
		}
	}

	return count;
}

static EditArmatureElement *nextEditArmatureElementChild(ListBase *list, EditArmatureElement *parent, int n)
{
	EditArmatureElement *eelement;

	for (eelement = list->first; eelement; eelement = eelement->next) {
		if (eelement->parent == parent) {
			if (n == 0) {
				return eelement;
>>>>>>> Initial commit
			}
			n--;
		}
	}
<<<<<<< HEAD
	
	return NULL;
}

static void getEditBoneRollUpAxis(EditBone *bone, float roll, float up_axis[3])
{
	float mat[3][3], nor[3];

	sub_v3_v3v3(nor, bone->tail, bone->head);
	
=======

	return NULL;
}

static void getEditArmatureElementRollUpAxis(EditArmatureElement *element, float roll, float up_axis[3])
{
	float mat[3][3], nor[3];

	sub_v3_v3v3(nor, element->tail, element->head);

>>>>>>> Initial commit
	vec_roll_to_mat3(nor, roll, mat);
	copy_v3_v3(up_axis, mat[2]);
}

<<<<<<< HEAD
static float rollBoneByQuatAligned(EditBone *bone, float old_up_axis[3], float qrot[4], float qroll[4], float aligned_axis[3])
{
	float nor[3], new_up_axis[3], x_axis[3], z_axis[3];
	
	copy_v3_v3(new_up_axis, old_up_axis);
	mul_qt_v3(qrot, new_up_axis);
	
	sub_v3_v3v3(nor, bone->tail, bone->head);
	
	cross_v3_v3v3(x_axis, nor, aligned_axis);
	cross_v3_v3v3(z_axis, x_axis, nor);
	
	normalize_v3(new_up_axis);
	normalize_v3(x_axis);
	normalize_v3(z_axis);
	
	if (dot_v3v3(new_up_axis, x_axis) < 0) {
		negate_v3(x_axis);
	}
	
	if (dot_v3v3(new_up_axis, z_axis) < 0) {
		negate_v3(z_axis);
	}
	
	if (angle_normalized_v3v3(x_axis, new_up_axis) < angle_normalized_v3v3(z_axis, new_up_axis)) {
		rotation_between_vecs_to_quat(qroll, new_up_axis, x_axis); /* set roll rotation quat */
		return ED_rollBoneToVector(bone, x_axis, false);
	}
	else {
		rotation_between_vecs_to_quat(qroll, new_up_axis, z_axis); /* set roll rotation quat */
		return ED_rollBoneToVector(bone, z_axis, false);
=======
static float rollBoneByQuatAligned(EditArmatureElement *element, float old_up_axis[3], float qrot[4], float qroll[4], float aligned_axis[3])
{
	float nor[3], new_up_axis[3], x_axis[3], z_axis[3];

	copy_v3_v3(new_up_axis, old_up_axis);
	mul_qt_v3(qrot, new_up_axis);

	sub_v3_v3v3(nor, element->tail, element->head);

	cross_v3_v3v3(x_axis, nor, aligned_axis);
	cross_v3_v3v3(z_axis, x_axis, nor);

	normalize_v3(new_up_axis);
	normalize_v3(x_axis);
	normalize_v3(z_axis);

	if (dot_v3v3(new_up_axis, x_axis) < 0) {
		negate_v3(x_axis);
	}

	if (dot_v3v3(new_up_axis, z_axis) < 0) {
		negate_v3(z_axis);
	}

	if (angle_normalized_v3v3(x_axis, new_up_axis) < angle_normalized_v3v3(z_axis, new_up_axis)) {
		rotation_between_vecs_to_quat(qroll, new_up_axis, x_axis); /* set roll rotation quat */
		return ED_rollElementToVector(element, x_axis, false);
	}
	else {
		rotation_between_vecs_to_quat(qroll, new_up_axis, z_axis); /* set roll rotation quat */
		return ED_rollElementToVector(element, z_axis, false);
>>>>>>> Initial commit
	}
}

static float rollBoneByQuatJoint(RigEdge *edge, RigEdge *previous, float qrot[4], float qroll[4], float up_axis[3])
{
	if (previous == NULL) {
		/* default to up_axis if no previous */
		return rollBoneByQuatAligned(edge->bone, edge->up_axis, qrot, qroll, up_axis);
	}
	else {
		float new_up_axis[3];
		float vec_first[3], vec_second[3], normal[3];
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		if (previous->bone) {
			sub_v3_v3v3(vec_first, previous->bone->tail, previous->bone->head);
		}
		else if (previous->prev->bone) {
			sub_v3_v3v3(vec_first, edge->bone->head, previous->prev->bone->tail);
		}
		else {
			/* default to up_axis if first bone in the chain is an offset */
			return rollBoneByQuatAligned(edge->bone, edge->up_axis, qrot, qroll, up_axis);
		}
<<<<<<< HEAD
		
		sub_v3_v3v3(vec_second, edge->bone->tail, edge->bone->head);
	
		normalize_v3(vec_first);
		normalize_v3(vec_second);
		
		cross_v3_v3v3(normal, vec_first, vec_second);
		normalize_v3(normal);
		
		axis_angle_to_quat(qroll, vec_second, edge->up_angle);
		
		mul_qt_v3(qroll, normal);
			
		copy_v3_v3(new_up_axis, edge->up_axis);
		mul_qt_v3(qrot, new_up_axis);
		
		normalize_v3(new_up_axis);
		
		/* real qroll between normal and up_axis */
		rotation_between_vecs_to_quat(qroll, new_up_axis, normal);

		return ED_rollBoneToVector(edge->bone, normal, false);
	}
}

float rollBoneByQuat(EditBone *bone, float old_up_axis[3], float qrot[4])
{
	float new_up_axis[3];
	
	copy_v3_v3(new_up_axis, old_up_axis);
	mul_qt_v3(qrot, new_up_axis);
	
	return ED_rollBoneToVector(bone, new_up_axis, false);
=======

		sub_v3_v3v3(vec_second, edge->bone->tail, edge->bone->head);

		normalize_v3(vec_first);
		normalize_v3(vec_second);

		cross_v3_v3v3(normal, vec_first, vec_second);
		normalize_v3(normal);

		axis_angle_to_quat(qroll, vec_second, edge->up_angle);

		mul_qt_v3(qroll, normal);

		copy_v3_v3(new_up_axis, edge->up_axis);
		mul_qt_v3(qrot, new_up_axis);

		normalize_v3(new_up_axis);

		/* real qroll between normal and up_axis */
		rotation_between_vecs_to_quat(qroll, new_up_axis, normal);

		return ED_rollElementToVector(edge->bone, normal, false);
	}
}

float rollBoneByQuat(EditArmatureElement *bone, float old_up_axis[3], float qrot[4])
{
	float new_up_axis[3];

	copy_v3_v3(new_up_axis, old_up_axis);
	mul_qt_v3(qrot, new_up_axis);

	return ED_rollElementToVector(bone, new_up_axis, false);
>>>>>>> Initial commit
}

/************************************ DESTRUCTORS ******************************************************/

static void RIG_freeRigArc(BArc *arc)
{
	BLI_freelistN(&((RigArc *)arc)->edges);
}

void RIG_freeRigGraph(BGraph *rg)
{
	RigGraph *rigg = (RigGraph *)rg;
	BNode *node;
	BArc *arc;
<<<<<<< HEAD
	
	BLI_task_pool_free(rigg->task_pool);
	BLI_task_scheduler_free(rigg->task_scheduler);
	
	if (rigg->link_mesh) {
		REEB_freeGraph(rigg->link_mesh);
	}
	
=======

	BLI_task_pool_free(rigg->task_pool);
	BLI_task_scheduler_free(rigg->task_scheduler);

	if (rigg->link_mesh) {
		REEB_freeGraph(rigg->link_mesh);
	}

>>>>>>> Initial commit
	for (arc = rg->arcs.first; arc; arc = arc->next) {
		RIG_freeRigArc(arc);
	}
	BLI_freelistN(&rg->arcs);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	for (node = rg->nodes.first; node; node = node->next) {
		BLI_freeNode(rg, (BNode *)node);
	}
	BLI_freelistN(&rg->nodes);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	BLI_freelistN(&rigg->controls);

	BLI_ghash_free(rigg->bones_map, NULL, NULL);
	BLI_ghash_free(rigg->controls_map, NULL, NULL);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	if (rigg->flag & RIG_FREE_BONELIST) {
		BLI_freelistN(rigg->editbones);
		MEM_freeN(rigg->editbones);
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	MEM_freeN(rg);
}

/************************************* ALLOCATORS ******************************************************/

static RigGraph *newRigGraph(void)
{
	RigGraph *rg;
	int totthread;
<<<<<<< HEAD
	
	rg = MEM_callocN(sizeof(RigGraph), "rig graph");
	
	rg->head = NULL;
	
	rg->bones_map = BLI_ghash_str_new("newRigGraph bones gh");
	rg->controls_map = BLI_ghash_str_new("newRigGraph cont gh");
	
	rg->free_arc = RIG_freeRigArc;
	rg->free_node = NULL;
	
=======

	rg = MEM_callocN(sizeof(RigGraph), "rig graph");

	rg->head = NULL;

	rg->bones_map = BLI_ghash_str_new("newRigGraph bones gh");
	rg->controls_map = BLI_ghash_str_new("newRigGraph cont gh");

	rg->free_arc = RIG_freeRigArc;
	rg->free_node = NULL;

>>>>>>> Initial commit
#ifdef USE_THREADS
	totthread = TASK_SCHEDULER_AUTO_THREADS;
#else
	totthread = TASK_SCHEDULER_SINGLE_THREAD;
#endif

	rg->task_scheduler = BLI_task_scheduler_create(totthread);
	rg->task_pool = BLI_task_pool_create(rg->task_scheduler, NULL);

	return rg;
}

static RigArc *newRigArc(RigGraph *rg)
{
	RigArc *arc;
<<<<<<< HEAD
	
	arc = MEM_callocN(sizeof(RigArc), "rig arc");
	arc->count = 0;
	BLI_addtail(&rg->arcs, arc);
	
=======

	arc = MEM_callocN(sizeof(RigArc), "rig arc");
	arc->count = 0;
	BLI_addtail(&rg->arcs, arc);

>>>>>>> Initial commit
	return arc;
}

static RigControl *newRigControl(RigGraph *rg)
{
	RigControl *ctrl;
<<<<<<< HEAD
	
	ctrl = MEM_callocN(sizeof(RigControl), "rig control");
	
	BLI_addtail(&rg->controls, ctrl);
	
=======

	ctrl = MEM_callocN(sizeof(RigControl), "rig control");

	BLI_addtail(&rg->controls, ctrl);

>>>>>>> Initial commit
	return ctrl;
}

static RigNode *newRigNodeHead(RigGraph *rg, RigArc *arc, float p[3])
{
	RigNode *node;
	node = MEM_callocN(sizeof(RigNode), "rig node");
	BLI_addtail(&rg->nodes, node);

	copy_v3_v3(node->p, p);
	node->degree = 1;
	node->arcs = NULL;
<<<<<<< HEAD
	
	arc->head = node;
	
=======

	arc->head = node;

>>>>>>> Initial commit
	return node;
}

static void addRigNodeHead(RigGraph *UNUSED(rg), RigArc *arc, RigNode *node)
{
	node->degree++;

	arc->head = node;
}

static RigNode *newRigNode(RigGraph *rg, float p[3])
{
	RigNode *node;
	node = MEM_callocN(sizeof(RigNode), "rig node");
	BLI_addtail(&rg->nodes, node);

	copy_v3_v3(node->p, p);
	node->degree = 0;
	node->arcs = NULL;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	return node;
}

static RigNode *newRigNodeTail(RigGraph *rg, RigArc *arc, float p[3])
{
	RigNode *node = newRigNode(rg, p);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	node->degree = 1;
	arc->tail = node;

	return node;
}

static void RIG_appendEdgeToArc(RigArc *arc, RigEdge *edge)
{
	BLI_addtail(&arc->edges, edge);

	if (edge->prev == NULL) {
		copy_v3_v3(edge->head, arc->head->p);
	}
	else {
		RigEdge *last_edge = edge->prev;
		copy_v3_v3(edge->head, last_edge->tail);
		RIG_calculateEdgeAngles(last_edge, edge);
	}
<<<<<<< HEAD
	
	edge->length = len_v3v3(edge->head, edge->tail);
	
	arc->length += edge->length;
	
	arc->count += 1;
}

static void RIG_addEdgeToArc(RigArc *arc, float tail[3], EditBone *bone)
=======

	edge->length = len_v3v3(edge->head, edge->tail);

	arc->length += edge->length;

	arc->count += 1;
}

static void RIG_addEdgeToArc(RigArc *arc, float tail[3], EditArmatureElement *bone)
>>>>>>> Initial commit
{
	RigEdge *edge;

	edge = MEM_callocN(sizeof(RigEdge), "rig edge");

	copy_v3_v3(edge->tail, tail);
	edge->bone = bone;
<<<<<<< HEAD
	
	if (bone) {
		getEditBoneRollUpAxis(bone, bone->roll, edge->up_axis);
	}
	
=======

	if (bone) {
		getEditArmatureElementRollUpAxis(bone, bone->roll, edge->up_axis);
	}

>>>>>>> Initial commit
	RIG_appendEdgeToArc(arc, edge);
}
/************************************** CLONING TEMPLATES **********************************************/

static void renameTemplateBone(char *name, char *template_name, ListBase *editbones, char *side_string, char *num_string)
{
	int i, j;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	for (i = 0, j = 0; i < (MAXBONENAME - 1) && j < (MAXBONENAME - 1) && template_name[i] != '\0'; i++) {
		if (template_name[i] == '&') {
			if (template_name[i + 1] == 'S' || template_name[i + 1] == 's') {
				j += BLI_strncpy_rlen(name + j, side_string, MAXBONENAME);
				i++;
			}
			else if (template_name[i + 1] == 'N' || template_name[i + 1] == 'n') {
				j += BLI_strncpy_rlen(name + j, num_string, MAXBONENAME);
				i++;
			}
			else {
				name[j] = template_name[i];
				j++;
			}
		}
		else {
			name[j] = template_name[i];
			j++;
		}
	}
<<<<<<< HEAD
	
	name[j] = '\0';
	
	unique_editbone_name(editbones, name, NULL);
=======

	name[j] = '\0';

	unique_editelement_name(editbones, name, NULL);
>>>>>>> Initial commit
}

static RigControl *cloneControl(RigGraph *rg, RigGraph *src_rg, RigControl *src_ctrl, GHash *ptr_hash, char *side_string, char *num_string)
{
	RigControl *ctrl;
	char name[MAXBONENAME];
<<<<<<< HEAD
	
	ctrl = newRigControl(rg);
	
=======

	ctrl = newRigControl(rg);

>>>>>>> Initial commit
	copy_v3_v3(ctrl->head, src_ctrl->head);
	copy_v3_v3(ctrl->tail, src_ctrl->tail);
	copy_v3_v3(ctrl->up_axis, src_ctrl->up_axis);
	copy_v3_v3(ctrl->offset, src_ctrl->offset);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	ctrl->tail_mode = src_ctrl->tail_mode;
	ctrl->flag = src_ctrl->flag;

	renameTemplateBone(name, src_ctrl->bone->name, rg->editbones, side_string, num_string);
<<<<<<< HEAD
	ctrl->bone = duplicateEditBoneObjects(src_ctrl->bone, name, rg->editbones, src_rg->ob, rg->ob);
	ctrl->bone->flag &= ~(BONE_TIPSEL | BONE_SELECTED | BONE_ROOTSEL);
	BLI_ghash_insert(ptr_hash, src_ctrl->bone, ctrl->bone);
	
	ctrl->link = src_ctrl->link;
	ctrl->link_tail = src_ctrl->link_tail;
	
=======
	ctrl->bone = duplicateEditArmatureElementObjects(src_ctrl->bone, name, rg->editbones, src_rg->ob, rg->ob);
	ctrl->bone->flag &= ~(ELEMENT_TIPSEL | ELEMENT_SELECTED | ELEMENT_ROOTSEL);
	BLI_ghash_insert(ptr_hash, src_ctrl->bone, ctrl->bone);

	ctrl->link = src_ctrl->link;
	ctrl->link_tail = src_ctrl->link_tail;

>>>>>>> Initial commit
	return ctrl;
}

static RigArc *cloneArc(RigGraph *rg, RigGraph *src_rg, RigArc *src_arc, GHash *ptr_hash, char *side_string, char *num_string)
{
	RigEdge *src_edge;
	RigArc  *arc;
<<<<<<< HEAD
	
	arc = newRigArc(rg);
	
	arc->head = BLI_ghash_lookup(ptr_hash, src_arc->head);
	arc->tail = BLI_ghash_lookup(ptr_hash, src_arc->tail);
	
	arc->head->degree++;
	arc->tail->degree++;
	
	arc->length = src_arc->length;

	arc->count = src_arc->count;
	
	for (src_edge = src_arc->edges.first; src_edge; src_edge = src_edge->next) {
		RigEdge *edge;
	
=======

	arc = newRigArc(rg);

	arc->head = BLI_ghash_lookup(ptr_hash, src_arc->head);
	arc->tail = BLI_ghash_lookup(ptr_hash, src_arc->tail);

	arc->head->degree++;
	arc->tail->degree++;

	arc->length = src_arc->length;

	arc->count = src_arc->count;

	for (src_edge = src_arc->edges.first; src_edge; src_edge = src_edge->next) {
		RigEdge *edge;

>>>>>>> Initial commit
		edge = MEM_callocN(sizeof(RigEdge), "rig edge");

		copy_v3_v3(edge->head, src_edge->head);
		copy_v3_v3(edge->tail, src_edge->tail);
		copy_v3_v3(edge->up_axis, src_edge->up_axis);
<<<<<<< HEAD
		
		edge->length = src_edge->length;
		edge->angle = src_edge->angle;
		edge->up_angle = src_edge->up_angle;
		
		if (src_edge->bone != NULL) {
			char name[MAXBONENAME];
			renameTemplateBone(name, src_edge->bone->name, rg->editbones, side_string, num_string);
			edge->bone = duplicateEditBoneObjects(src_edge->bone, name, rg->editbones, src_rg->ob, rg->ob);
			edge->bone->flag &= ~(BONE_TIPSEL | BONE_SELECTED | BONE_ROOTSEL);
=======

		edge->length = src_edge->length;
		edge->angle = src_edge->angle;
		edge->up_angle = src_edge->up_angle;

		if (src_edge->bone != NULL) {
			char name[MAXBONENAME];
			renameTemplateBone(name, src_edge->bone->name, rg->editbones, side_string, num_string);
			edge->bone = duplicateEditArmatureElementObjects(src_edge->bone, name, rg->editbones, src_rg->ob, rg->ob);
			edge->bone->flag &= ~(ELEMENT_TIPSEL | ELEMENT_SELECTED | ELEMENT_ROOTSEL);
>>>>>>> Initial commit
			BLI_ghash_insert(ptr_hash, src_edge->bone, edge->bone);
		}

		BLI_addtail(&arc->edges, edge);
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	return arc;
}

static RigGraph *cloneRigGraph(RigGraph *src, ListBase *editbones, Object *ob, char *side_string, char *num_string)
{
	GHash   *ptr_hash;
	RigNode *node;
	RigArc  *arc;
	RigControl *ctrl;
	RigGraph *rg;
<<<<<<< HEAD
	
	ptr_hash = BLI_ghash_ptr_new("cloneRigGraph gh");

	rg = newRigGraph();
	
	rg->ob = ob;
	rg->editbones = editbones;
	
	preEditBoneDuplicate(rg->editbones); /* prime bones for duplication */
	preEditBoneDuplicate(src->editbones); /* prime bones for duplication */
	
=======

	ptr_hash = BLI_ghash_ptr_new("cloneRigGraph gh");

	rg = newRigGraph();

	rg->ob = ob;
	rg->editbones = editbones;

	preEditArmatureElementDuplicate(rg->editbones); /* prime bones for duplication */
	preEditArmatureElementDuplicate(src->editbones); /* prime bones for duplication */

>>>>>>> Initial commit
	/* Clone nodes */
	for (node = src->nodes.first; node; node = node->next) {
		RigNode *cloned_node = newRigNode(rg, node->p);
		BLI_ghash_insert(ptr_hash, node, cloned_node);
	}
<<<<<<< HEAD
	
	rg->head = BLI_ghash_lookup(ptr_hash, src->head);
	
=======

	rg->head = BLI_ghash_lookup(ptr_hash, src->head);

>>>>>>> Initial commit
	/* Clone arcs */
	for (arc = src->arcs.first; arc; arc = arc->next) {
		cloneArc(rg, src, arc, ptr_hash, side_string, num_string);
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* Clone controls */
	for (ctrl = src->controls.first; ctrl; ctrl = ctrl->next) {
		cloneControl(rg, src, ctrl, ptr_hash, side_string, num_string);
	}
<<<<<<< HEAD
	
	/* Relink bones properly */
	for (arc = rg->arcs.first; arc; arc = arc->next) {
		RigEdge *edge;
		
		for (edge = arc->edges.first; edge; edge = edge->next) {
			if (edge->bone != NULL) {
				EditBone *bone;
				
=======

	/* Relink bones properly */
	for (arc = rg->arcs.first; arc; arc = arc->next) {
		RigEdge *edge;

		for (edge = arc->edges.first; edge; edge = edge->next) {
			if (edge->bone != NULL) {
				EditArmatureElement *bone;

>>>>>>> Initial commit
				updateDuplicateSubtargetObjects(edge->bone, src->editbones, src->ob, rg->ob);

				if (edge->bone->parent) {
					bone = BLI_ghash_lookup(ptr_hash, edge->bone->parent);
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
					if (bone != NULL) {
						edge->bone->parent = bone;
					}
					else {
						/* disconnect since parent isn't cloned
<<<<<<< HEAD
						 * this will only happen when cloning from selected bones 
						 * */
						edge->bone->flag &= ~BONE_CONNECTED;
=======
						 * this will only happen when cloning from selected bones
						 * */
						edge->bone->flag &= ~ELEMENT_CONNECTED;
>>>>>>> Initial commit
					}
				}
			}
		}
	}
<<<<<<< HEAD
	
	for (ctrl = rg->controls.first; ctrl; ctrl = ctrl->next) {
		EditBone *bone;
		
=======

	for (ctrl = rg->controls.first; ctrl; ctrl = ctrl->next) {
		EditArmatureElement *bone;

>>>>>>> Initial commit
		updateDuplicateSubtargetObjects(ctrl->bone, src->editbones, src->ob, rg->ob);

		if (ctrl->bone->parent) {
			bone = BLI_ghash_lookup(ptr_hash, ctrl->bone->parent);
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			if (bone != NULL) {
				ctrl->bone->parent = bone;
			}
			else {
				/* disconnect since parent isn't cloned
<<<<<<< HEAD
				 * this will only happen when cloning from selected bones 
				 * */
				ctrl->bone->flag &= ~BONE_CONNECTED;
=======
				 * this will only happen when cloning from selected bones
				 * */
				ctrl->bone->flag &= ~ELEMENT_CONNECTED;
>>>>>>> Initial commit
			}
		}

		ctrl->link = BLI_ghash_lookup(ptr_hash, ctrl->link);
		ctrl->link_tail = BLI_ghash_lookup(ptr_hash, ctrl->link_tail);
	}
<<<<<<< HEAD
	
	BLI_ghash_free(ptr_hash, NULL, NULL);
	
=======

	BLI_ghash_free(ptr_hash, NULL, NULL);

>>>>>>> Initial commit
	return rg;
}


/*******************************************************************************************************/

static void RIG_calculateEdgeAngles(RigEdge *edge_first, RigEdge *edge_second)
{
	float vec_first[3], vec_second[3];
<<<<<<< HEAD
	
	sub_v3_v3v3(vec_first, edge_first->tail, edge_first->head); 
=======

	sub_v3_v3v3(vec_first, edge_first->tail, edge_first->head);
>>>>>>> Initial commit
	sub_v3_v3v3(vec_second, edge_second->tail, edge_second->head);

	normalize_v3(vec_first);
	normalize_v3(vec_second);
<<<<<<< HEAD
	
	edge_first->angle = angle_normalized_v3v3(vec_first, vec_second);
	
=======

	edge_first->angle = angle_normalized_v3v3(vec_first, vec_second);

>>>>>>> Initial commit
	if (edge_second->bone != NULL) {
		float normal[3];

		cross_v3_v3v3(normal, vec_first, vec_second);
		normalize_v3(normal);

		edge_second->up_angle = angle_normalized_v3v3(normal, edge_second->up_axis);
	}
}

/************************************ CONTROL BONES ****************************************************/

<<<<<<< HEAD
static void RIG_addControlBone(RigGraph *rg, EditBone *bone)
=======
static void RIG_addControlBone(RigGraph *rg, EditArmatureElement *bone)
>>>>>>> Initial commit
{
	RigControl *ctrl = newRigControl(rg);
	ctrl->bone = bone;
	copy_v3_v3(ctrl->head, bone->head);
	copy_v3_v3(ctrl->tail, bone->tail);
<<<<<<< HEAD
	getEditBoneRollUpAxis(bone, bone->roll, ctrl->up_axis);
	ctrl->tail_mode = TL_NONE;
	
	BLI_ghash_insert(rg->controls_map, bone->name, ctrl);
}

static int RIG_parentControl(RigControl *ctrl, EditBone *link)
=======
	getEditArmatureElementRollUpAxis(bone, bone->roll, ctrl->up_axis);
	ctrl->tail_mode = TL_NONE;

	BLI_ghash_insert(rg->controls_map, bone->name, ctrl);
}

static int RIG_parentControl(RigControl *ctrl, EditArmatureElement *link)
>>>>>>> Initial commit
{
	if (link) {
		float offset[3];
		int flag = 0;
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		sub_v3_v3v3(offset, ctrl->bone->head, link->head);

		/* if root matches, check for direction too */
		if (dot_v3v3(offset, offset) < 0.0001f) {
			float vbone[3], vparent[3];
<<<<<<< HEAD
			
			flag |= RIG_CTRL_FIT_ROOT;
			
			sub_v3_v3v3(vbone, ctrl->bone->tail, ctrl->bone->head);
			sub_v3_v3v3(vparent, link->tail, link->head);
			
=======

			flag |= RIG_CTRL_FIT_ROOT;

			sub_v3_v3v3(vbone, ctrl->bone->tail, ctrl->bone->head);
			sub_v3_v3v3(vparent, link->tail, link->head);

>>>>>>> Initial commit
			/* test for opposite direction */
			if (dot_v3v3(vbone, vparent) > 0) {
				float nor[3];
				float len;
<<<<<<< HEAD
				
				cross_v3_v3v3(nor, vbone, vparent);
				
=======

				cross_v3_v3v3(nor, vbone, vparent);

>>>>>>> Initial commit
				len = dot_v3v3(nor, nor);
				if (len < 0.0001f) {
					flag |= RIG_CTRL_FIT_BONE;
				}
			}
		}
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		/* Bail out if old one is automatically better */
		if (flag < ctrl->flag) {
			return 0;
		}
<<<<<<< HEAD
		
		/* if there's already a link
		 *  overwrite only if new link is higher in the chain */
		if (ctrl->link && flag == ctrl->flag) {
			EditBone *bone = NULL;
			
=======

		/* if there's already a link
		 *  overwrite only if new link is higher in the chain */
		if (ctrl->link && flag == ctrl->flag) {
			EditArmatureElement *bone = NULL;

>>>>>>> Initial commit
			for (bone = ctrl->link; bone; bone = bone->parent) {
				/* if link is in the chain, break and use that one */
				if (bone == link) {
					break;
				}
			}
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			/* not in chain, don't update link */
			if (bone == NULL) {
				return 0;
			}
		}
<<<<<<< HEAD
		
		
		ctrl->link = link;
		ctrl->flag = flag;
		
		copy_v3_v3(ctrl->offset, offset);
		
		return 1;
	}
	
=======


		ctrl->link = link;
		ctrl->flag = flag;

		copy_v3_v3(ctrl->offset, offset);

		return 1;
	}

>>>>>>> Initial commit
	return 0;
}

static void RIG_reconnectControlBones(RigGraph *rg)
{
	RigControl *ctrl;
	bool changed = true;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* first pass, link to deform bones */
	for (ctrl = rg->controls.first; ctrl; ctrl = ctrl->next) {
		bPoseChannel *pchan;
		bConstraint *con;
		int found = 0;
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		/* DO SOME MAGIC HERE */
		for (pchan = rg->ob->pose->chanbase.first; pchan; pchan = pchan->next) {
			for (con = pchan->constraints.first; con; con = con->next) {
				bConstraintTypeInfo *cti = BKE_constraint_typeinfo_get(con);
				ListBase targets = {NULL, NULL};
				bConstraintTarget *ct;
<<<<<<< HEAD
				
				/* constraint targets */
				if (cti && cti->get_constraint_targets) {
					int target_index;
					
					cti->get_constraint_targets(con, &targets);
					
					for (target_index = 0, ct = targets.first; ct; target_index++, ct = ct->next) {
						if ((ct->tar == rg->ob) && STREQ(ct->subtarget, ctrl->bone->name)) {
							/* SET bone link to bone corresponding to pchan */
							EditBone *link = BLI_ghash_lookup(rg->bones_map, pchan->name);
							
=======

				/* constraint targets */
				if (cti && cti->get_constraint_targets) {
					int target_index;

					cti->get_constraint_targets(con, &targets);

					for (target_index = 0, ct = targets.first; ct; target_index++, ct = ct->next) {
						if ((ct->tar == rg->ob) && strcmp(ct->subtarget, ctrl->bone->name) == 0) {
							/* SET bone link to bone corresponding to pchan */
							EditArmatureElement *link = BLI_ghash_lookup(rg->bones_map, pchan->name);

>>>>>>> Initial commit
							/* Making sure bone is in this armature */
							if (link != NULL) {
								/* for pole targets, link to parent bone instead, if possible */
								if (con->type == CONSTRAINT_TYPE_KINEMATIC && target_index == 1) {
									if (link->parent && BLI_ghash_haskey(rg->bones_map, link->parent->name)) {
										link = link->parent;
									}
								}
<<<<<<< HEAD
								
=======

>>>>>>> Initial commit
								found = RIG_parentControl(ctrl, link);
							}
						}
					}
<<<<<<< HEAD
					
=======

>>>>>>> Initial commit
					if (cti->flush_constraint_targets)
						cti->flush_constraint_targets(con, &targets, 0);
				}
			}
		}

		/* if not found yet, check parent */
		if (found == 0) {
			if (ctrl->bone->parent) {
				/* make sure parent is a deforming bone
				 * NULL if not
				 *  */
<<<<<<< HEAD
				EditBone *link = BLI_ghash_lookup(rg->bones_map, ctrl->bone->parent->name);
				
				found = RIG_parentControl(ctrl, link);
			}
			
=======
				EditArmatureElement *link = BLI_ghash_lookup(rg->bones_map, ctrl->bone->parent->name);

				found = RIG_parentControl(ctrl, link);
			}

>>>>>>> Initial commit
			/* check if bone is not superposed on another one */
			{
				RigArc *arc;
				RigArc *best_arc = NULL;
<<<<<<< HEAD
				EditBone *link = NULL;
				
=======
				EditArmatureElement *link = NULL;

>>>>>>> Initial commit
				for (arc = rg->arcs.first; arc; arc = arc->next) {
					RigEdge *edge;
					for (edge = arc->edges.first; edge; edge = edge->next) {
						if (edge->bone) {
							int fit = 0;
<<<<<<< HEAD
							
							fit = len_v3v3(ctrl->bone->head, edge->bone->head) < 0.0001f;
							fit = fit || len_v3v3(ctrl->bone->tail, edge->bone->tail) < 0.0001f;
							
=======

							fit = len_v3v3(ctrl->bone->head, edge->bone->head) < 0.0001f;
							fit = fit || len_v3v3(ctrl->bone->tail, edge->bone->tail) < 0.0001f;

>>>>>>> Initial commit
							if (fit) {
								/* pick the bone on the arc with the lowest symmetry level
								 * means you connect control to the trunk of the skeleton */
								if (best_arc == NULL || arc->symmetry_level < best_arc->symmetry_level) {
									best_arc = arc;
									link = edge->bone;
								}
							}
						}
					}
				}
<<<<<<< HEAD
				
				found = RIG_parentControl(ctrl, link);
			}
		}
		
=======

				found = RIG_parentControl(ctrl, link);
			}
		}

>>>>>>> Initial commit
		/* if not found yet, check child */
		if (found == 0) {
			RigArc *arc;
			RigArc *best_arc = NULL;
<<<<<<< HEAD
			EditBone *link = NULL;
			
=======
			EditArmatureElement *link = NULL;

>>>>>>> Initial commit
			for (arc = rg->arcs.first; arc; arc = arc->next) {
				RigEdge *edge;
				for (edge = arc->edges.first; edge; edge = edge->next) {
					if (edge->bone && edge->bone->parent == ctrl->bone) {
						/* pick the bone on the arc with the lowest symmetry level
						 * means you connect control to the trunk of the skeleton */
						if (best_arc == NULL || arc->symmetry_level < best_arc->symmetry_level) {
							best_arc = arc;
							link = edge->bone;
						}
					}
				}
			}
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			found = RIG_parentControl(ctrl, link);
		}

	}
<<<<<<< HEAD
	
	
	/* second pass, make chains in control bones */
	while (changed) {
		changed = false;
		
=======


	/* second pass, make chains in control bones */
	while (changed) {
		changed = false;

>>>>>>> Initial commit
		for (ctrl = rg->controls.first; ctrl; ctrl = ctrl->next) {
			/* if control is not linked yet */
			if (ctrl->link == NULL) {
				bPoseChannel *pchan;
				bConstraint *con;
				RigControl *ctrl_parent = NULL;
				RigControl *ctrl_child;
				int found = 0;

				if (ctrl->bone->parent) {
					ctrl_parent = BLI_ghash_lookup(rg->controls_map, ctrl->bone->parent->name);
				}

				/* check constraints first */
<<<<<<< HEAD
				
=======

>>>>>>> Initial commit
				/* DO SOME MAGIC HERE */
				for (pchan = rg->ob->pose->chanbase.first; pchan; pchan = pchan->next) {
					for (con = pchan->constraints.first; con; con = con->next) {
						bConstraintTypeInfo *cti = BKE_constraint_typeinfo_get(con);
						ListBase targets = {NULL, NULL};
						bConstraintTarget *ct;
<<<<<<< HEAD
						
						/* constraint targets */
						if (cti && cti->get_constraint_targets) {
							cti->get_constraint_targets(con, &targets);
							
							for (ct = targets.first; ct; ct = ct->next) {
								if ((ct->tar == rg->ob) && STREQ(ct->subtarget, ctrl->bone->name)) {
=======

						/* constraint targets */
						if (cti && cti->get_constraint_targets) {
							cti->get_constraint_targets(con, &targets);

							for (ct = targets.first; ct; ct = ct->next) {
								if ((ct->tar == rg->ob) && strcmp(ct->subtarget, ctrl->bone->name) == 0) {
>>>>>>> Initial commit
									/* SET bone link to ctrl corresponding to pchan */
									RigControl *link = BLI_ghash_lookup(rg->controls_map, pchan->name);

									/* if owner is a control bone, link with it */
									if (link && link->link) {
										RIG_parentControl(ctrl, link->bone);
										found = 1;
										break;
									}
								}
							}
<<<<<<< HEAD
							
=======

>>>>>>> Initial commit
							if (cti->flush_constraint_targets)
								cti->flush_constraint_targets(con, &targets, 0);
						}
					}
				}

				if (found == 0) {
					/* check if parent is already linked */
					if (ctrl_parent && ctrl_parent->link) {
						RIG_parentControl(ctrl, ctrl_parent->bone);
						changed = true;
					}
					else {
						/* check childs */
						for (ctrl_child = rg->controls.first; ctrl_child; ctrl_child = ctrl_child->next) {
							/* if a child is linked, link to that one */
							if (ctrl_child->link && ctrl_child->bone->parent == ctrl->bone) {
								RIG_parentControl(ctrl, ctrl_child->bone);
								changed = true;
								break;
							}
						}
					}
				}
			}
		}
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* third pass, link control tails */
	for (ctrl = rg->controls.first; ctrl; ctrl = ctrl->next) {
		/* fit bone already means full match, so skip those */
		if ((ctrl->flag & RIG_CTRL_FIT_BONE) == 0) {
			GHashIterator ghi;
<<<<<<< HEAD
			
			/* look on deform bones first */
			BLI_ghashIterator_init(&ghi, rg->bones_map);
			
			for (; !BLI_ghashIterator_done(&ghi); BLI_ghashIterator_step(&ghi)) {
				EditBone *bone = (EditBone *)BLI_ghashIterator_getValue(&ghi);
				
=======

			/* look on deform bones first */
			BLI_ghashIterator_init(&ghi, rg->bones_map);

			for (; !BLI_ghashIterator_done(&ghi); BLI_ghashIterator_step(&ghi)) {
				EditArmatureElement *bone = (EditArmatureElement *)BLI_ghashIterator_getValue(&ghi);

>>>>>>> Initial commit
				/* don't link with parent */
				if (bone->parent != ctrl->bone) {
					if (len_v3v3(ctrl->bone->tail, bone->head) < 0.01f) {
						ctrl->tail_mode = TL_HEAD;
						ctrl->link_tail = bone;
						break;
					}
					else if (len_v3v3(ctrl->bone->tail, bone->tail) < 0.01f) {
						ctrl->tail_mode = TL_TAIL;
						ctrl->link_tail = bone;
						break;
					}
				}
			}
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			/* if we haven't found one yet, look in control bones */
			if (ctrl->tail_mode == TL_NONE) {
			}
		}
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
}

/*******************************************************************************************************/

static void RIG_joinArcs(RigGraph *rg, RigNode *node, RigArc *joined_arc1, RigArc *joined_arc2)
{
	RigEdge *edge, *next_edge;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* ignore cases where joint is at start or end */
	if (joined_arc1->head == joined_arc2->head || joined_arc1->tail == joined_arc2->tail) {
		return;
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* swap arcs to make sure arc1 is before arc2 */
	if (joined_arc1->head == joined_arc2->tail) {
		RigArc *tmp = joined_arc1;
		joined_arc1 = joined_arc2;
		joined_arc2 = tmp;
	}
<<<<<<< HEAD
	
	for (edge = joined_arc2->edges.first; edge; edge = next_edge) {
		next_edge = edge->next;
		
		RIG_appendEdgeToArc(joined_arc1, edge);
	}
	
	joined_arc1->tail = joined_arc2->tail;
	
	BLI_listbase_clear(&joined_arc2->edges);
	
	BLI_removeArc((BGraph *)rg, (BArc *)joined_arc2);
	
=======

	for (edge = joined_arc2->edges.first; edge; edge = next_edge) {
		next_edge = edge->next;

		RIG_appendEdgeToArc(joined_arc1, edge);
	}

	joined_arc1->tail = joined_arc2->tail;

	BLI_listbase_clear(&joined_arc2->edges);

	BLI_removeArc((BGraph *)rg, (BArc *)joined_arc2);

>>>>>>> Initial commit
	BLI_removeNode((BGraph *)rg, (BNode *)node);
}

static void RIG_removeNormalNodes(RigGraph *rg)
{
	RigNode *node, *next_node;
<<<<<<< HEAD
	
	for (node = rg->nodes.first; node; node = next_node) {
		next_node = node->next;
		
		if (node->degree == 2) {
			RigArc *arc, *joined_arc1 = NULL, *joined_arc2 = NULL;
			
=======

	for (node = rg->nodes.first; node; node = next_node) {
		next_node = node->next;

		if (node->degree == 2) {
			RigArc *arc, *joined_arc1 = NULL, *joined_arc2 = NULL;

>>>>>>> Initial commit
			for (arc = rg->arcs.first; arc; arc = arc->next) {
				if (arc->head == node || arc->tail == node) {
					if (joined_arc1 == NULL) {
						joined_arc1 = arc;
					}
					else {
						joined_arc2 = arc;
						break;
					}
				}
			}
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			RIG_joinArcs(rg, node, joined_arc1, joined_arc2);
		}
	}
}

static void RIG_removeUneededOffsets(RigGraph *rg)
{
	RigArc *arc;
<<<<<<< HEAD
	
	for (arc = rg->arcs.first; arc; arc = arc->next) {
		RigEdge *first_edge, *last_edge;
		
		first_edge = arc->edges.first;
		last_edge = arc->edges.last;
		
=======

	for (arc = rg->arcs.first; arc; arc = arc->next) {
		RigEdge *first_edge, *last_edge;

		first_edge = arc->edges.first;
		last_edge = arc->edges.last;

>>>>>>> Initial commit
		if (first_edge->bone == NULL) {
			if (first_edge->bone == NULL && len_v3v3(first_edge->tail, arc->head->p) <= 0.001f) {
				BLI_remlink(&arc->edges, first_edge);
				MEM_freeN(first_edge);
			}
			else if (arc->head->degree == 1) {
				RigNode *new_node = (RigNode *)BLI_FindNodeByPosition((BGraph *)rg, first_edge->tail, 0.001f);
<<<<<<< HEAD
				
=======

>>>>>>> Initial commit
				if (new_node) {
					BLI_remlink(&arc->edges, first_edge);
					MEM_freeN(first_edge);
					BLI_replaceNodeInArc((BGraph *)rg, (BArc *)arc, (BNode *)new_node, (BNode *)arc->head);
				}
				else {
					RigEdge *next_edge = first_edge->next;
<<<<<<< HEAD
	
					if (next_edge) {
						BLI_remlink(&arc->edges, first_edge);
						MEM_freeN(first_edge);
						
=======

					if (next_edge) {
						BLI_remlink(&arc->edges, first_edge);
						MEM_freeN(first_edge);

>>>>>>> Initial commit
						copy_v3_v3(arc->head->p, next_edge->head);
					}
				}
			}
			else {
				/* check if all arc connected start with a null edge */
				RigArc *other_arc;
				for (other_arc = rg->arcs.first; other_arc; other_arc = other_arc->next) {
					if (other_arc != arc) {
						RigEdge *test_edge;
						if (other_arc->head == arc->head) {
							test_edge = other_arc->edges.first;
<<<<<<< HEAD
							
=======

>>>>>>> Initial commit
							if (test_edge->bone != NULL) {
								break;
							}
						}
						else if (other_arc->tail == arc->head) {
							test_edge = other_arc->edges.last;
<<<<<<< HEAD
							
=======

>>>>>>> Initial commit
							if (test_edge->bone != NULL) {
								break;
							}
						}
					}
				}
<<<<<<< HEAD
				
				if (other_arc == NULL) {
					RigNode *new_node = (RigNode *)BLI_FindNodeByPosition((BGraph *)rg, first_edge->tail, 0.001);
					
=======

				if (other_arc == NULL) {
					RigNode *new_node = (RigNode *)BLI_FindNodeByPosition((BGraph *)rg, first_edge->tail, 0.001);

>>>>>>> Initial commit
					if (new_node) {
						/* remove null edge in other arcs too */
						for (other_arc = rg->arcs.first; other_arc; other_arc = other_arc->next) {
							if (other_arc != arc) {
								RigEdge *test_edge;
								if (other_arc->head == arc->head) {
									BLI_replaceNodeInArc((BGraph *)rg, (BArc *)other_arc, (BNode *)new_node, (BNode *)other_arc->head);
									test_edge = other_arc->edges.first;
									BLI_remlink(&other_arc->edges, test_edge);
									MEM_freeN(test_edge);
								}
								else if (other_arc->tail == arc->head) {
									BLI_replaceNodeInArc((BGraph *)rg, (BArc *)other_arc, (BNode *)new_node, (BNode *)other_arc->tail);
									test_edge = other_arc->edges.last;
									BLI_remlink(&other_arc->edges, test_edge);
									MEM_freeN(test_edge);
								}
							}
						}
<<<<<<< HEAD
						
=======

>>>>>>> Initial commit
						BLI_remlink(&arc->edges, first_edge);
						MEM_freeN(first_edge);
						BLI_replaceNodeInArc((BGraph *)rg, (BArc *)arc, (BNode *)new_node, (BNode *)arc->head);
					}
					else {
						RigEdge *next_edge = first_edge->next;
<<<<<<< HEAD
		
						if (next_edge) {
							BLI_remlink(&arc->edges, first_edge);
							MEM_freeN(first_edge);
							
							copy_v3_v3(arc->head->p, next_edge->head);
							
=======

						if (next_edge) {
							BLI_remlink(&arc->edges, first_edge);
							MEM_freeN(first_edge);

							copy_v3_v3(arc->head->p, next_edge->head);

>>>>>>> Initial commit
							/* remove null edge in other arcs too */
							for (other_arc = rg->arcs.first; other_arc; other_arc = other_arc->next) {
								if (other_arc != arc) {
									RigEdge *test_edge;
									if (other_arc->head == arc->head) {
										test_edge = other_arc->edges.first;
										BLI_remlink(&other_arc->edges, test_edge);
										MEM_freeN(test_edge);
									}
									else if (other_arc->tail == arc->head) {
										test_edge = other_arc->edges.last;
										BLI_remlink(&other_arc->edges, test_edge);
										MEM_freeN(test_edge);
									}
								}
							}
						}
					}
				}
			}
		}
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		if (last_edge->bone == NULL) {
			if (len_v3v3(last_edge->head, arc->tail->p) <= 0.001f) {
				BLI_remlink(&arc->edges, last_edge);
				MEM_freeN(last_edge);
			}
			else if (arc->tail->degree == 1) {
				RigNode *new_node = (RigNode *)BLI_FindNodeByPosition((BGraph *)rg, last_edge->head, 0.001f);
<<<<<<< HEAD
				
				if (new_node) {
					RigEdge *previous_edge = last_edge->prev;
					
					BLI_remlink(&arc->edges, last_edge);
					MEM_freeN(last_edge);
					BLI_replaceNodeInArc((BGraph *)rg, (BArc *)arc, (BNode *)new_node, (BNode *)arc->tail);
					
=======

				if (new_node) {
					RigEdge *previous_edge = last_edge->prev;

					BLI_remlink(&arc->edges, last_edge);
					MEM_freeN(last_edge);
					BLI_replaceNodeInArc((BGraph *)rg, (BArc *)arc, (BNode *)new_node, (BNode *)arc->tail);

>>>>>>> Initial commit
					/* set previous angle to 0, since there's no following edges */
					if (previous_edge) {
						previous_edge->angle = 0;
					}
				}
				else {
					RigEdge *previous_edge = last_edge->prev;
<<<<<<< HEAD
	
					if (previous_edge) {
						BLI_remlink(&arc->edges, last_edge);
						MEM_freeN(last_edge);
						
=======

					if (previous_edge) {
						BLI_remlink(&arc->edges, last_edge);
						MEM_freeN(last_edge);

>>>>>>> Initial commit
						copy_v3_v3(arc->tail->p, previous_edge->tail);
						previous_edge->angle = 0;
					}
				}
			}
		}
	}
}

<<<<<<< HEAD
static void RIG_arcFromBoneChain(RigGraph *rg, ListBase *list, EditBone *root_bone, RigNode *starting_node, bool selected)
{
	EditBone *bone, *last_bone = root_bone;
	RigArc *arc = NULL;
	int contain_head = 0;
	
	for (bone = root_bone; bone; bone = nextEditBoneChild(list, bone, 0)) {
		int nb_children;
		
		if (selected == 0 || (bone->flag & BONE_SELECTED)) {
			if ((bone->flag & BONE_NO_DEFORM) == 0) {
				BLI_ghash_insert(rg->bones_map, bone->name, bone);
			
				if (arc == NULL) {
					arc = newRigArc(rg);
					
					if (starting_node == NULL) {
						starting_node = newRigNodeHead(rg, arc, root_bone->head);
=======
static void RIG_arcFromBoneChain(RigGraph *rg, ListBase *list, EditArmatureElement *root_element, RigNode *starting_node, bool selected)
{
	EditArmatureElement *element, *last_element = root_element;
	RigArc *arc = NULL;
	int contain_head = 0;

	for (element = root_element; element; element = nextEditArmatureElementChild(list, element, 0)) {
		int nb_children;

		if (selected == 0 || (element->flag & ELEMENT_SELECTED)) {
			if ((element->flag & ELEMENT_NO_DEFORM) == 0) {
				BLI_ghash_insert(rg->bones_map, element->name, element);

				if (arc == NULL) {
					arc = newRigArc(rg);

					if (starting_node == NULL) {
						starting_node = newRigNodeHead(rg, arc, root_element->head);
>>>>>>> Initial commit
					}
					else {
						addRigNodeHead(rg, arc, starting_node);
					}
				}
<<<<<<< HEAD
				
				if (bone->parent && (bone->flag & BONE_CONNECTED) == 0) {
					RIG_addEdgeToArc(arc, bone->head, NULL);
				}
				
				RIG_addEdgeToArc(arc, bone->tail, bone);
				
				last_bone = bone;
				
				if (STREQ(bone->name, "head")) {
					contain_head = 1;
				}
			}
			else if ((bone->flag & BONE_EDITMODE_LOCKED) == 0) { /* ignore locked bones */
				RIG_addControlBone(rg, bone);
			}
		}
		
		nb_children = countEditBoneChildren(list, bone);
		if (nb_children > 1) {
			RigNode *end_node = NULL;
			int i;
			
			if (arc != NULL) {
				end_node = newRigNodeTail(rg, arc, bone->tail);
			}
			else {
				end_node = newRigNode(rg, bone->tail);
			}

			for (i = 0; i < nb_children; i++) {
				root_bone = nextEditBoneChild(list, bone, i);
				RIG_arcFromBoneChain(rg, list, root_bone, end_node, selected);
			}
			
=======

				if (element->parent && (element->flag & ELEMENT_CONNECTED) == 0) {
					RIG_addEdgeToArc(arc, element->head, NULL);
				}

				RIG_addEdgeToArc(arc, element->tail, element);

				last_element = element;

				if (strcmp(element->name, "head") == 0) {
					contain_head = 1;
				}
			}
			else if ((element->flag & ELEMENT_EDITMODE_LOCKED) == 0) { /* ignore locked bones */
				RIG_addControlBone(rg, element);
			}
		}

		nb_children = countEditArmatureElementChildren(list, element);
		if (nb_children > 1) {
			RigNode *end_node = NULL;
			int i;

			if (arc != NULL) {
				end_node = newRigNodeTail(rg, arc, element->tail);
			}
			else {
				end_node = newRigNode(rg, element->tail);
			}

			for (i = 0; i < nb_children; i++) {
				root_element = nextEditArmatureElementChild(list, element, i);
				RIG_arcFromBoneChain(rg, list, root_element, end_node, selected);
			}

>>>>>>> Initial commit
			/* arc ends here, break */
			break;
		}
	}
<<<<<<< HEAD
	
	/* If the loop exited without forking */
	if (arc != NULL && bone == NULL) {
		newRigNodeTail(rg, arc, last_bone->tail);
=======

	/* If the loop exited without forking */
	if (arc != NULL && element == NULL) {
		newRigNodeTail(rg, arc, last_element->tail);
>>>>>>> Initial commit
	}

	if (contain_head) {
		rg->head = arc->tail;
	}
}

/*******************************************************************************************************/
static void RIG_findHead(RigGraph *rg)
{
	if (rg->head == NULL) {
		if (BLI_listbase_is_single(&rg->arcs)) {
			RigArc *arc = rg->arcs.first;
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			rg->head = (RigNode *)arc->head;
		}
		else {
			RigArc *arc;
<<<<<<< HEAD
			
			for (arc = rg->arcs.first; arc; arc = arc->next) {
				RigEdge *edge = arc->edges.last;
				
				if (edge->bone->flag & (BONE_TIPSEL | BONE_SELECTED)) {
=======

			for (arc = rg->arcs.first; arc; arc = arc->next) {
				RigEdge *edge = arc->edges.last;

				if (edge->bone->flag & (ELEMENT_TIPSEL | ELEMENT_SELECTED)) {
>>>>>>> Initial commit
					rg->head = arc->tail;
					break;
				}
			}
		}
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		if (rg->head == NULL) {
			rg->head = rg->nodes.first;
		}
	}
}

/*******************************************************************************************************/

static void RIG_printNode(RigNode *node, const char name[])
{
	printf("%s %p %i <%0.3f, %0.3f, %0.3f>\n", name, (void *)node, node->degree, node->p[0], node->p[1], node->p[2]);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	if (node->symmetry_flag & SYM_TOPOLOGICAL) {
		if (node->symmetry_flag & SYM_AXIAL)
			printf("Symmetry AXIAL\n");
		else if (node->symmetry_flag & SYM_RADIAL)
			printf("Symmetry RADIAL\n");
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
		print_v3("symmetry axis", node->symmetry_axis);
	}
}

void RIG_printArcBones(RigArc *arc)
{
	RigEdge *edge;

	for (edge = arc->edges.first; edge; edge = edge->next) {
		if (edge->bone)
			printf("%s ", edge->bone->name);
		else
			printf("---- ");
	}
	printf("\n");
}

static void RIG_printCtrl(RigControl *ctrl, char *indent)
{
	char text[128];
<<<<<<< HEAD
	
	printf("%sBone: %s\n", indent, ctrl->bone->name);
	printf("%sLink: %s\n", indent, ctrl->link ? ctrl->link->name : "!NONE!");
	
	BLI_snprintf(text, sizeof(text), "%soffset", indent);
	print_v3(text, ctrl->offset);
	
	printf("%sFlag: %i\n", indent, ctrl->flag);
}

static void RIG_printLinkedCtrl(RigGraph *rg, EditBone *bone, int tabs)
=======

	printf("%sBone: %s\n", indent, ctrl->bone->name);
	printf("%sLink: %s\n", indent, ctrl->link ? ctrl->link->name : "!NONE!");

	BLI_snprintf(text, sizeof(text), "%soffset", indent);
	print_v3(text, ctrl->offset);

	printf("%sFlag: %i\n", indent, ctrl->flag);
}

static void RIG_printLinkedCtrl(RigGraph *rg, EditArmatureElement *bone, int tabs)
>>>>>>> Initial commit
{
	RigControl *ctrl;
	char indent[64];
	char *s = indent;
	int i;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	for (i = 0; i < tabs; i++) {
		s[0] = '\t';
		s++;
	}
	s[0] = 0;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	for (ctrl = rg->controls.first; ctrl; ctrl = ctrl->next) {
		if (ctrl->link == bone) {
			RIG_printCtrl(ctrl, indent);
			RIG_printLinkedCtrl(rg, ctrl->bone, tabs + 1);
		}
	}
}

void RIG_printArc(RigGraph *rg, RigArc *arc)
{
	RigEdge *edge;

	RIG_printNode((RigNode *)arc->head, "head");

	for (edge = arc->edges.first; edge; edge = edge->next) {
		printf("\tinner joints %0.3f %0.3f %0.3f\n", edge->tail[0], edge->tail[1], edge->tail[2]);
		printf("\t\tlength %f\n", edge->length);
		printf("\t\tangle %f\n", edge->angle * (float)(180 / M_PI));
		if (edge->bone) {
			printf("\t\t%s\n", edge->bone->name);
			RIG_printLinkedCtrl(rg, edge->bone, 3);
		}
	}
	printf("symmetry level: %i flag: %i group %i\n", arc->symmetry_level, arc->symmetry_flag, arc->symmetry_group);

	RIG_printNode((RigNode *)arc->tail, "tail");
}

void RIG_printGraph(RigGraph *rg)
{
	RigArc *arc;

	printf("---- ARCS ----\n");
	for (arc = rg->arcs.first; arc; arc = arc->next) {
		RIG_printArc(rg, arc);
		printf("\n");
	}

	if (rg->head) {
		RIG_printNode(rg->head, "HEAD NODE:");
	}
	else {
		printf("HEAD NODE: NONE\n");
	}
}

/*******************************************************************************************************/

RigGraph *RIG_graphFromArmature(const bContext *C, Object *ob, bArmature *arm)
{
	Object *obedit = CTX_data_edit_object(C);
	Scene *scene = CTX_data_scene(C);
<<<<<<< HEAD
	EditBone *ebone;
	RigGraph *rg;

	rg = newRigGraph();
	
=======
	EditArmatureElement *eelement;
	RigGraph *rg;

	rg = newRigGraph();

>>>>>>> Initial commit
	if (obedit == ob) {
		rg->editbones = ((bArmature *)obedit->data)->edbo;
	}
	else {
		rg->editbones = MEM_callocN(sizeof(ListBase), "EditBones");
<<<<<<< HEAD
		make_boneList(rg->editbones, &arm->bonebase, NULL, NULL);
		rg->flag |= RIG_FREE_BONELIST;
	}
	
	rg->ob = ob;

	/* Do the rotations */
	for (ebone = rg->editbones->first; ebone; ebone = ebone->next) {
		if (ebone->parent == NULL) {
			RIG_arcFromBoneChain(rg, rg->editbones, ebone, NULL, 0);
		}
	}
	
	BLI_removeDoubleNodes((BGraph *)rg, 0.001);
	
	RIG_removeNormalNodes(rg);
	
	RIG_removeUneededOffsets(rg);
	
	BLI_buildAdjacencyList((BGraph *)rg);
	
	RIG_findHead(rg);

	BLI_markdownSymmetry((BGraph *)rg, (BNode *)rg->head, scene->toolsettings->skgen_symmetry_limit);
	
	RIG_reconnectControlBones(rg); /* after symmetry, because we use levels to find best match */
	
	if (BLI_isGraphCyclic((BGraph *)rg)) {
		printf("armature cyclic\n");
	}
	
=======
		make_elementList(rg->editbones, &arm->bonebase, NULL, NULL);
		rg->flag |= RIG_FREE_BONELIST;
	}

	rg->ob = ob;

	/* Do the rotations */
	for (eelement = rg->editbones->first; eelement; eelement = eelement->next) {
		if (eelement->parent == NULL) {
			RIG_arcFromBoneChain(rg, rg->editbones, eelement, NULL, 0);
		}
	}

	BLI_removeDoubleNodes((BGraph *)rg, 0.001);

	RIG_removeNormalNodes(rg);

	RIG_removeUneededOffsets(rg);

	BLI_buildAdjacencyList((BGraph *)rg);

	RIG_findHead(rg);

	BLI_markdownSymmetry((BGraph *)rg, (BNode *)rg->head, scene->toolsettings->skgen_symmetry_limit);

	RIG_reconnectControlBones(rg); /* after symmetry, because we use levels to find best match */

	if (BLI_isGraphCyclic((BGraph *)rg)) {
		printf("armature cyclic\n");
	}

>>>>>>> Initial commit
	return rg;
}

static RigGraph *armatureSelectedToGraph(bContext *C, Object *ob, bArmature *arm)
{
	Object *obedit = CTX_data_edit_object(C);
	Scene *scene = CTX_data_scene(C);
<<<<<<< HEAD
	EditBone *ebone;
	RigGraph *rg;

	rg = newRigGraph();
	
=======
	EditArmatureElement *eelement;
	RigGraph *rg;

	rg = newRigGraph();

>>>>>>> Initial commit
	if (obedit == ob) {
		rg->editbones = arm->edbo;
	}
	else {
		rg->editbones = MEM_callocN(sizeof(ListBase), "EditBones");
<<<<<<< HEAD
		make_boneList(rg->editbones, &arm->bonebase, NULL, NULL);
=======
		make_elementList(rg->editbones, &arm->bonebase, NULL, NULL);
>>>>>>> Initial commit
		rg->flag |= RIG_FREE_BONELIST;
	}

	rg->ob = ob;

	/* Do the rotations */
<<<<<<< HEAD
	for (ebone = rg->editbones->first; ebone; ebone = ebone->next) {
		if (ebone->parent == NULL) {
			RIG_arcFromBoneChain(rg, rg->editbones, ebone, NULL, 1);
		}
	}
	
	BLI_removeDoubleNodes((BGraph *)rg, 0.001);
	
	RIG_removeNormalNodes(rg);
	
	RIG_removeUneededOffsets(rg);
	
	BLI_buildAdjacencyList((BGraph *)rg);
	
	RIG_findHead(rg);

	BLI_markdownSymmetry((BGraph *)rg, (BNode *)rg->head, scene->toolsettings->skgen_symmetry_limit);
	
	RIG_reconnectControlBones(rg); /* after symmetry, because we use levels to find best match */
	
	if (BLI_isGraphCyclic((BGraph *)rg)) {
		printf("armature cyclic\n");
	}
	
=======
	for (eelement = rg->editbones->first; eelement; eelement = eelement->next) {
		if (eelement->parent == NULL) {
			RIG_arcFromBoneChain(rg, rg->editbones, eelement, NULL, 1);
		}
	}

	BLI_removeDoubleNodes((BGraph *)rg, 0.001);

	RIG_removeNormalNodes(rg);

	RIG_removeUneededOffsets(rg);

	BLI_buildAdjacencyList((BGraph *)rg);

	RIG_findHead(rg);

	BLI_markdownSymmetry((BGraph *)rg, (BNode *)rg->head, scene->toolsettings->skgen_symmetry_limit);

	RIG_reconnectControlBones(rg); /* after symmetry, because we use levels to find best match */

	if (BLI_isGraphCyclic((BGraph *)rg)) {
		printf("armature cyclic\n");
	}

>>>>>>> Initial commit
	return rg;
}
/************************************ GENERATING *****************************************************/

#if 0
static EditBone *add_editbonetolist(char *name, ListBase *list)
{
	EditBone *bone = MEM_callocN(sizeof(EditBone), "eBone");
<<<<<<< HEAD
	
	BLI_strncpy(bone->name, name, sizeof(bone->name));
	unique_editbone_name(list, bone->name, NULL);
	
	BLI_addtail(list, bone);
	
=======

	BLI_strncpy(bone->name, name, sizeof(bone->name));
	unique_editelement_name(list, bone->name, NULL);

	BLI_addtail(list, bone);

>>>>>>> Initial commit
	bone->flag |= BONE_TIPSEL;
	bone->weight = 1.0F;
	bone->dist = 0.25F;
	bone->xwidth = 0.1;
	bone->zwidth = 0.1;
	bone->ease1 = 1.0;
	bone->ease2 = 1.0;
	bone->rad_head = 0.10;
	bone->rad_tail = 0.05;
	bone->segments = 1;
	bone->layer =  1; //arm->layer;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	return bone;
}
#endif

#if 0 /* UNUSED */
static void generateMissingArcsFromNode(RigGraph *rigg, ReebNode *node, int multi_level_limit)
{
	while (node->multi_level > multi_level_limit && node->link_up)
	{
		node = node->link_up;
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	while (node->multi_level < multi_level_limit && node->link_down)
	{
		node = node->link_down;
	}
<<<<<<< HEAD
	
	if (node->multi_level == multi_level_limit)
	{
		int i;
		
		for (i = 0; i < node->degree; i++)
		{
			ReebArc *earc = node->arcs[i];
			
			if (earc->flag == ARC_FREE && earc->head == node)
			{
				ReebNode *other = BIF_otherNodeFromIndex(earc, node);
				
				earc->flag = ARC_USED;
				
=======

	if (node->multi_level == multi_level_limit)
	{
		int i;

		for (i = 0; i < node->degree; i++)
		{
			ReebArc *earc = node->arcs[i];

			if (earc->flag == ARC_FREE && earc->head == node)
			{
				ReebNode *other = BIF_otherNodeFromIndex(earc, node);

				earc->flag = ARC_USED;

>>>>>>> Initial commit
				//generateBonesForArc(rigg, earc, node, other);
				generateMissingArcsFromNode(rigg, other, multi_level_limit);
			}
		}
	}
}

static void generateMissingArcs(RigGraph *rigg)
{
	ReebGraph *reebg;
	int multi_level_limit = 5;
<<<<<<< HEAD
	
	for (reebg = rigg->link_mesh; reebg; reebg = reebg->link_up)
	{
		ReebArc *earc;
		
=======

	for (reebg = rigg->link_mesh; reebg; reebg = reebg->link_up)
	{
		ReebArc *earc;

>>>>>>> Initial commit
		for (earc = reebg->arcs.first; earc; earc = earc->next)
		{
			if (earc->flag == ARC_USED)
			{
				generateMissingArcsFromNode(rigg, earc->head, multi_level_limit);
				generateMissingArcsFromNode(rigg, earc->tail, multi_level_limit);
			}
		}
	}
}
#endif

/************************************ RETARGETTING *****************************************************/

static void repositionControl(RigGraph *rigg, RigControl *ctrl, float head[3], float tail[3], float qrot[4], float resize);

static void repositionTailControl(RigGraph *rigg, RigControl *ctrl);

static void finalizeControl(RigGraph *rigg, RigControl *ctrl, float resize)
{
	if ((ctrl->flag & RIG_CTRL_DONE) == RIG_CTRL_DONE) {
		RigControl *ctrl_child;

<<<<<<< HEAD
#if 0		
		printf("CTRL: %s LINK: %s", ctrl->bone->name, ctrl->link->name);
		
=======
#if 0
		printf("CTRL: %s LINK: %s", ctrl->bone->name, ctrl->link->name);

>>>>>>> Initial commit
		if (ctrl->link_tail)
		{
			printf(" TAIL: %s", ctrl->link_tail->name);
		}
<<<<<<< HEAD
		
		printf("\n");
#endif
		
=======

		printf("\n");
#endif

>>>>>>> Initial commit
		/* if there was a tail link: apply link, recalc resize factor and qrot */
		if (ctrl->tail_mode != TL_NONE) {
			float *tail_vec = NULL;
			float v1[3], v2[3], qtail[4];
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			if (ctrl->tail_mode == TL_TAIL) {
				tail_vec = ctrl->link_tail->tail;
			}
			else if (ctrl->tail_mode == TL_HEAD) {
				tail_vec = ctrl->link_tail->head;
			}
<<<<<<< HEAD
			
			sub_v3_v3v3(v1, ctrl->bone->tail, ctrl->bone->head);
			sub_v3_v3v3(v2, tail_vec, ctrl->bone->head);
			
			copy_v3_v3(ctrl->bone->tail, tail_vec);
			
			rotation_between_vecs_to_quat(qtail, v1, v2);
			mul_qt_qtqt(ctrl->qrot, qtail, ctrl->qrot);
			
			resize = len_v3(v2) / len_v3v3(ctrl->head, ctrl->tail);
		}
		
		ctrl->bone->roll = rollBoneByQuat(ctrl->bone, ctrl->up_axis, ctrl->qrot);
	
=======

			sub_v3_v3v3(v1, ctrl->bone->tail, ctrl->bone->head);
			sub_v3_v3v3(v2, tail_vec, ctrl->bone->head);

			copy_v3_v3(ctrl->bone->tail, tail_vec);

			rotation_between_vecs_to_quat(qtail, v1, v2);
			mul_qt_qtqt(ctrl->qrot, qtail, ctrl->qrot);

			resize = len_v3(v2) / len_v3v3(ctrl->head, ctrl->tail);
		}

		ctrl->bone->roll = rollBoneByQuat(ctrl->bone, ctrl->up_axis, ctrl->qrot);

>>>>>>> Initial commit
		/* Cascade to connected control bones */
		for (ctrl_child = rigg->controls.first; ctrl_child; ctrl_child = ctrl_child->next) {
			if (ctrl_child->link == ctrl->bone) {
				repositionControl(rigg, ctrl_child, ctrl->bone->head, ctrl->bone->tail, ctrl->qrot, resize);
			}
			if (ctrl_child->link_tail == ctrl->bone) {
				repositionTailControl(rigg, ctrl_child);
			}
		}
	}
}

static void repositionTailControl(RigGraph *rigg, RigControl *ctrl)
{
	ctrl->flag |= RIG_CTRL_TAIL_DONE;

	finalizeControl(rigg, ctrl, 1); /* resize will be recalculated anyway so we don't need it */
}

static void repositionControl(RigGraph *rigg, RigControl *ctrl, float head[3], float UNUSED(tail[3]), float qrot[4], float resize)
{
	float parent_offset[3], tail_offset[3];
<<<<<<< HEAD
	
	copy_v3_v3(parent_offset, ctrl->offset);
	mul_v3_fl(parent_offset, resize);
	mul_qt_v3(qrot, parent_offset);
	
	add_v3_v3v3(ctrl->bone->head, head, parent_offset); 
=======

	copy_v3_v3(parent_offset, ctrl->offset);
	mul_v3_fl(parent_offset, resize);
	mul_qt_v3(qrot, parent_offset);

	add_v3_v3v3(ctrl->bone->head, head, parent_offset);
>>>>>>> Initial commit

	ctrl->flag |= RIG_CTRL_HEAD_DONE;

	copy_qt_qt(ctrl->qrot, qrot);

	if (ctrl->tail_mode == TL_NONE) {
		sub_v3_v3v3(tail_offset, ctrl->tail, ctrl->head);
		mul_v3_fl(tail_offset, resize);
		mul_qt_v3(qrot, tail_offset);

		add_v3_v3v3(ctrl->bone->tail, ctrl->bone->head, tail_offset);
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		ctrl->flag |= RIG_CTRL_TAIL_DONE;
	}

	finalizeControl(rigg, ctrl, resize);
}

static void repositionBone(bContext *C, RigGraph *rigg, RigEdge *edge, float vec0[3], float vec1[3], float up_axis[3])
{
	Scene *scene = CTX_data_scene(C);
<<<<<<< HEAD
	EditBone *bone;
=======
	EditArmatureElement *bone;
>>>>>>> Initial commit
	RigControl *ctrl;
	float qrot[4], resize;
	float v1[3], v2[3];
	float l1, l2;
<<<<<<< HEAD
	
	bone = edge->bone;
	
	sub_v3_v3v3(v1, edge->tail, edge->head);
	sub_v3_v3v3(v2, vec1, vec0);
	
=======

	bone = edge->bone;

	sub_v3_v3v3(v1, edge->tail, edge->head);
	sub_v3_v3v3(v2, vec1, vec0);

>>>>>>> Initial commit
	l1 = normalize_v3(v1);
	l2 = normalize_v3(v2);

	resize = l2 / l1;
<<<<<<< HEAD
	
	rotation_between_vecs_to_quat(qrot, v1, v2);
	
	copy_v3_v3(bone->head, vec0);
	copy_v3_v3(bone->tail, vec1);
	
=======

	rotation_between_vecs_to_quat(qrot, v1, v2);

	copy_v3_v3(bone->head, vec0);
	copy_v3_v3(bone->tail, vec1);

>>>>>>> Initial commit
	if (!is_zero_v3(up_axis)) {
		float qroll[4];

		if (scene->toolsettings->skgen_retarget_roll == SK_RETARGET_ROLL_VIEW) {
			bone->roll = rollBoneByQuatAligned(bone, edge->up_axis, qrot, qroll, up_axis);
		}
		else if (scene->toolsettings->skgen_retarget_roll == SK_RETARGET_ROLL_JOINT) {
			bone->roll = rollBoneByQuatJoint(edge, edge->prev, qrot, qroll, up_axis);
		}
		else {
			unit_qt(qroll);
		}
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		mul_qt_qtqt(qrot, qroll, qrot);
	}
	else {
		bone->roll = rollBoneByQuat(bone, edge->up_axis, qrot);
	}

	for (ctrl = rigg->controls.first; ctrl; ctrl = ctrl->next) {
		if (ctrl->link == bone) {
			repositionControl(rigg, ctrl, vec0, vec1, qrot, resize);
		}
		if (ctrl->link_tail == bone) {
			repositionTailControl(rigg, ctrl);
		}
	}
}

static RetargetMode detectArcRetargetMode(RigArc *arc);
static void retargetArctoArcLength(bContext *C, RigGraph *rigg, RigArc *iarc, RigNode *inode_start);


static RetargetMode detectArcRetargetMode(RigArc *iarc)
{
	RetargetMode mode = RETARGET_AGGRESSIVE;
	ReebArc *earc = iarc->link_mesh;
	RigEdge *edge;
	int large_angle = 0;
	float avg_angle = 0;
	/* float avg_length = 0; */ /* UNUSED */
	int nb_edges = 0;
<<<<<<< HEAD
	
	
=======


>>>>>>> Initial commit
	for (edge = iarc->edges.first; edge; edge = edge->next) {
		avg_angle += edge->angle;
		nb_edges++;
	}
<<<<<<< HEAD
	
	avg_angle /= nb_edges - 1; /* -1 because last edge doesn't have an angle */

	/* avg_length = iarc->length / nb_edges; */  /* UNUSED */
	
	
=======

	avg_angle /= nb_edges - 1; /* -1 because last edge doesn't have an angle */

	/* avg_length = iarc->length / nb_edges; */  /* UNUSED */


>>>>>>> Initial commit
	if (nb_edges > 2) {
		for (edge = iarc->edges.first; edge; edge = edge->next) {
			if (fabsf(edge->angle - avg_angle) > (float)(M_PI / 6)) {
				large_angle = 1;
			}
		}
	}
	else if (nb_edges == 2 && avg_angle > 0) {
		large_angle = 1;
	}
<<<<<<< HEAD
		
	
	if (large_angle == 0) {
		mode = RETARGET_LENGTH;
	}
	
	if (earc->bcount <= (iarc->count - 1)) {
		mode = RETARGET_LENGTH;
	}
	
=======


	if (large_angle == 0) {
		mode = RETARGET_LENGTH;
	}

	if (earc->bcount <= (iarc->count - 1)) {
		mode = RETARGET_LENGTH;
	}

>>>>>>> Initial commit
	return mode;
}

#ifndef USE_THREADS
static void printMovesNeeded(int *positions, int nb_positions)
{
	int moves = 0;
	int i;
<<<<<<< HEAD
	
	for (i = 0; i < nb_positions; i++) {
		moves += positions[i] - (i + 1);
	}
	
=======

	for (i = 0; i < nb_positions; i++) {
		moves += positions[i] - (i + 1);
	}

>>>>>>> Initial commit
	printf("%i moves needed\n", moves);
}

static void printPositions(int *positions, int nb_positions)
{
	int i;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	for (i = 0; i < nb_positions; i++) {
		printf("%i ", positions[i]);
	}
	printf("\n");
}
#endif

#define MAX_COST FLT_MAX /* FIX ME */

static float costDistance(BArcIterator *iter, float *vec0, float *vec1, int i0, int i1, float distance_weight)
{
	EmbedBucket *bucket = NULL;
	float max_dist = 0;
	float v1[3], v2[3], c[3];
	float v1_inpf;

	if (distance_weight > 0) {
		sub_v3_v3v3(v1, vec0, vec1);
<<<<<<< HEAD
		
		v1_inpf = dot_v3v3(v1, v1);
		
=======

		v1_inpf = dot_v3v3(v1, v1);

>>>>>>> Initial commit
		if (v1_inpf > 0) {
			int j;
			for (j = i0 + 1; j < i1 - 1; j++) {
				float dist;
<<<<<<< HEAD
				
				bucket = IT_peek(iter, j);
	
				sub_v3_v3v3(v2, bucket->p, vec1);
		
				cross_v3_v3v3(c, v1, v2);
				
				dist = dot_v3v3(c, c) / v1_inpf;
				
				max_dist = dist > max_dist ? dist : max_dist;
			}
			
=======

				bucket = IT_peek(iter, j);

				sub_v3_v3v3(v2, bucket->p, vec1);

				cross_v3_v3v3(c, v1, v2);

				dist = dot_v3v3(c, c) / v1_inpf;

				max_dist = dist > max_dist ? dist : max_dist;
			}

>>>>>>> Initial commit
			return distance_weight * max_dist;
		}
		else {
			return MAX_COST;
		}
	}
	else {
		return 0;
	}
}

static float costAngle(float original_angle, float vec_first[3], float vec_second[3], float angle_weight)
{
	if (angle_weight > 0) {
		float current_angle;
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		if (!is_zero_v3(vec_first) && !is_zero_v3(vec_second)) {
			current_angle = saacos(dot_v3v3(vec_first, vec_second));

			return angle_weight * fabsf(current_angle - original_angle);
		}
		else {
			return angle_weight * (float)M_PI;
		}
	}
	else {
		return 0;
	}
}

static float costLength(float original_length, float current_length, float length_weight)
{
	if (current_length == 0) {
		return MAX_COST;
	}
	else {
		float length_ratio = fabsf((current_length - original_length) / original_length);
		return length_weight * length_ratio * length_ratio;
	}
}

#if 0
static float calcCostLengthDistance(BArcIterator *iter, float **vec_cache, RigEdge *edge, float *vec1, float *vec2, int i1, int i2)
{
	float vec[3];
	float length;

	sub_v3_v3v3(vec, vec2, vec1);
	length = normalize_v3(vec);

	return costLength(edge->length, length) + costDistance(iter, vec1, vec2, i1, i2);
}
#endif

static float calcCostAngleLengthDistance(BArcIterator *iter, float **UNUSED(vec_cache), RigEdge *edge,
                                         float *vec0, float *vec1, float *vec2, int i1, int i2,
                                         float angle_weight, float length_weight, float distance_weight)
{
	float vec_second[3], vec_first[3];
	float length2;
	float new_cost = 0;

	sub_v3_v3v3(vec_second, vec2, vec1);
	length2 = normalize_v3(vec_second);


	/* Angle cost */
	if (edge->prev) {
<<<<<<< HEAD
		sub_v3_v3v3(vec_first, vec1, vec0); 
		normalize_v3(vec_first);
		
=======
		sub_v3_v3v3(vec_first, vec1, vec0);
		normalize_v3(vec_first);

>>>>>>> Initial commit
		new_cost += costAngle(edge->prev->angle, vec_first, vec_second, angle_weight);
	}

	/* Length cost */
	new_cost += costLength(edge->length, length2, length_weight);

	/* Distance cost */
	new_cost += costDistance(iter, vec1, vec2, i1, i2, distance_weight);

	return new_cost;
}

static int indexMemoNode(int nb_positions, int previous, int current, int joints_left)
{
	return joints_left * nb_positions * nb_positions + current * nb_positions + previous;
}

static void copyMemoPositions(int *positions, MemoNode *table, int nb_positions, int joints_left)
{
	int previous = 0, current = 0;
	int i = 0;
<<<<<<< HEAD
	
	for (i = 0; joints_left > 0; joints_left--, i++) {
		MemoNode *node;
		node = table + indexMemoNode(nb_positions, previous, current, joints_left);
		
		positions[i] = node->next;
		
=======

	for (i = 0; joints_left > 0; joints_left--, i++) {
		MemoNode *node;
		node = table + indexMemoNode(nb_positions, previous, current, joints_left);

		positions[i] = node->next;

>>>>>>> Initial commit
		previous = current;
		current = node->next;
	}
}

static MemoNode *solveJoints(MemoNode *table, BArcIterator *iter, float **vec_cache,
                             int nb_joints, int nb_positions, int previous, int current, RigEdge *edge,
                             int joints_left, float angle_weight, float length_weight, float distance_weight)
{
	MemoNode *node;
	int index = indexMemoNode(nb_positions, previous, current, joints_left);
<<<<<<< HEAD
	
	node = table + index;
	
=======

	node = table + index;

>>>>>>> Initial commit
	if (node->weight != 0) {
		return node;
	}
	else if (joints_left == 0) {
		float *vec0 = vec_cache[previous];
		float *vec1 = vec_cache[current];
		float *vec2 = vec_cache[nb_positions + 1];

		node->weight = calcCostAngleLengthDistance(iter, vec_cache, edge, vec0, vec1, vec2, current, iter->length, angle_weight, length_weight, distance_weight);

		return node;
	}
	else {
		MemoNode *min_node = NULL;
		float *vec0 = vec_cache[previous];
		float *vec1 = vec_cache[current];
		float min_weight = 0.0f;
		int min_next = 0;
		int next;
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		for (next = current + 1; next <= nb_positions - (joints_left - 1); next++) {
			MemoNode *next_node;
			float *vec2 = vec_cache[next];
			float weight = 0.0f;
<<<<<<< HEAD
			
			/* ADD WEIGHT OF PREVIOUS - CURRENT - NEXT triple */
			weight = calcCostAngleLengthDistance(iter, vec_cache, edge, vec0, vec1, vec2, current, next, angle_weight, length_weight, distance_weight);
			
			if (weight >= MAX_COST) {
				continue;
			}
			
			/* add node weight */
			next_node = solveJoints(table, iter, vec_cache, nb_joints, nb_positions, current, next, edge->next, joints_left - 1, angle_weight, length_weight, distance_weight);
			weight += next_node->weight;
			
=======

			/* ADD WEIGHT OF PREVIOUS - CURRENT - NEXT triple */
			weight = calcCostAngleLengthDistance(iter, vec_cache, edge, vec0, vec1, vec2, current, next, angle_weight, length_weight, distance_weight);

			if (weight >= MAX_COST) {
				continue;
			}

			/* add node weight */
			next_node = solveJoints(table, iter, vec_cache, nb_joints, nb_positions, current, next, edge->next, joints_left - 1, angle_weight, length_weight, distance_weight);
			weight += next_node->weight;

>>>>>>> Initial commit
			if (min_node == NULL || weight < min_weight) {
				min_weight = weight;
				min_node = next_node;
				min_next = next;
			}
		}
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		if (min_node) {
			node->weight = min_weight;
			node->next = min_next;
			return node;
		}
		else {
			node->weight = MAX_COST;
			return node;
		}
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
}

static int testFlipArc(RigArc *iarc, RigNode *inode_start)
{
	ReebArc *earc = iarc->link_mesh;
	ReebNode *enode_start = BIF_NodeFromIndex(earc, inode_start->link_mesh);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* no flip needed if both nodes are the same */
	if ((enode_start == earc->head && inode_start == iarc->head) ||
	    (enode_start == earc->tail && inode_start == iarc->tail))
	{
		return 0;
	}
	else {
		return 1;
	}
}

static void retargetArctoArcAggresive(bContext *C, RigGraph *rigg, RigArc *iarc, RigNode *inode_start)
{
	ReebArcIterator arc_iter;
	BArcIterator *iter = (BArcIterator *)&arc_iter;
	RigEdge *edge;
	ReebNode *node_start, *node_end;
	ReebArc *earc = iarc->link_mesh;
	float angle_weight = 1.0; // GET FROM CONTEXT
	float length_weight = 1.0;
	float distance_weight = 1.0;
#ifndef USE_THREADS
	float min_cost = FLT_MAX;
#endif
	float *vec0, *vec1;
	int *best_positions;
	int nb_edges = BLI_listbase_count(&iarc->edges);
	int nb_joints = nb_edges - 1;
	RetargetMethod method = METHOD_MEMOIZE;
	int i;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	if (nb_joints > earc->bcount) {
		printf("NOT ENOUGH BUCKETS!\n");
		return;
	}

	best_positions = MEM_callocN(sizeof(int) * nb_joints, "Best positions");
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	if (testFlipArc(iarc, inode_start)) {
		node_start = earc->tail;
		node_end = earc->head;
	}
	else {
		node_start = earc->head;
		node_end = earc->tail;
	}

	/* equal number of joints and potential position, just fill them in */
	if (nb_joints == earc->bcount) {
		/* init with first values */
		for (i = 0; i < nb_joints; i++) {
			best_positions[i] = i + 1;
		}
	}
	if (method == METHOD_MEMOIZE) {
		int nb_positions = earc->bcount;
		int nb_memo_nodes = nb_positions * nb_positions * (nb_joints + 1);
		MemoNode *table = MEM_callocN(nb_memo_nodes * sizeof(MemoNode), "memoization table");
#ifndef USE_THREADS
		MemoNode *result;
#endif
		float **positions_cache = MEM_callocN(sizeof(float *) * (nb_positions + 2), "positions cache");
<<<<<<< HEAD
		
		positions_cache[0] = node_start->p;
		positions_cache[nb_positions + 1] = node_end->p;
		
=======

		positions_cache[0] = node_start->p;
		positions_cache[nb_positions + 1] = node_end->p;

>>>>>>> Initial commit
		initArcIterator(iter, earc, node_start);

		for (i = 1; i <= nb_positions; i++) {
			EmbedBucket *bucket = IT_peek(iter, i);
			positions_cache[i] = bucket->p;
		}

#ifndef USE_THREADS
		result = solveJoints(table, iter, positions_cache, nb_joints, earc->bcount, 0, 0, iarc->edges.first, nb_joints, angle_weight, length_weight, distance_weight);
		min_cost = result->weight;
#else
		solveJoints(table, iter, positions_cache, nb_joints, earc->bcount, 0, 0, iarc->edges.first, nb_joints, angle_weight, length_weight, distance_weight);
#endif

		copyMemoPositions(best_positions, table, earc->bcount, nb_joints);

		MEM_freeN(table);
		MEM_freeN(positions_cache);
	}

	vec0 = node_start->p;
	initArcIterator(iter, earc, node_start);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
#ifndef USE_THREADS
	printPositions(best_positions, nb_joints);
	printMovesNeeded(best_positions, nb_joints);
	printf("min_cost %f\n", min_cost);
	printf("buckets: %i\n", earc->bcount);
#endif

	/* set joints to best position */
	for (edge = iarc->edges.first, i = 0;
	     edge;
	     edge = edge->next, i++)
	{
		float *no = NULL;
		if (i < nb_joints) {
			EmbedBucket *bucket = IT_peek(iter, best_positions[i]);
			vec1 = bucket->p;
			no = bucket->no;
		}
		else {
			vec1 = node_end->p;
			no = node_end->no;
		}
<<<<<<< HEAD
		
		if (edge->bone) {
			repositionBone(C, rigg, edge, vec0, vec1, no);
		}
		
=======

		if (edge->bone) {
			repositionBone(C, rigg, edge, vec0, vec1, no);
		}

>>>>>>> Initial commit
		vec0 = vec1;
	}

	MEM_freeN(best_positions);
}

static void retargetArctoArcLength(bContext *C, RigGraph *rigg, RigArc *iarc, RigNode *inode_start)
{
	ReebArcIterator arc_iter;
	BArcIterator *iter = (BArcIterator *)&arc_iter;
	ReebArc *earc = iarc->link_mesh;
	ReebNode *node_start, *node_end;
	RigEdge *edge;
	EmbedBucket *bucket = NULL;
	float embedding_length = 0;
	float *vec0 = NULL;
	float *vec1 = NULL;
	float *previous_vec = NULL;

<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	if (testFlipArc(iarc, inode_start)) {
		node_start = (ReebNode *)earc->tail;
		node_end = (ReebNode *)earc->head;
	}
	else {
		node_start = (ReebNode *)earc->head;
		node_end = (ReebNode *)earc->tail;
	}
<<<<<<< HEAD
	
	initArcIterator(iter, earc, node_start);

	bucket = IT_next(iter);
	
	vec0 = node_start->p;
	
	while (bucket != NULL) {
		vec1 = bucket->p;
		
		embedding_length += len_v3v3(vec0, vec1);
		
		vec0 = vec1;
		bucket = IT_next(iter);
	}
	
	embedding_length += len_v3v3(node_end->p, vec1);
	
=======

	initArcIterator(iter, earc, node_start);

	bucket = IT_next(iter);

	vec0 = node_start->p;

	while (bucket != NULL) {
		vec1 = bucket->p;

		embedding_length += len_v3v3(vec0, vec1);

		vec0 = vec1;
		bucket = IT_next(iter);
	}

	embedding_length += len_v3v3(node_end->p, vec1);

>>>>>>> Initial commit
	/* fit bones */
	initArcIterator(iter, earc, node_start);

	bucket = IT_next(iter);

	vec0 = node_start->p;
	previous_vec = vec0;
	vec1 = bucket->p;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	for (edge = iarc->edges.first; edge; edge = edge->next) {
		float new_bone_length = edge->length / iarc->length * embedding_length;
		float *no = NULL;
		float length = 0;

		while (bucket && new_bone_length > length) {
			length += len_v3v3(previous_vec, vec1);
			bucket = IT_next(iter);
			previous_vec = vec1;
			vec1 = bucket->p;
			no = bucket->no;
		}
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		if (bucket == NULL) {
			vec1 = node_end->p;
			no = node_end->no;
		}

		/* no need to move virtual edges (space between unconnected bones) */
		if (edge->bone) {
			repositionBone(C, rigg, edge, vec0, vec1, no);
		}
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		vec0 = vec1;
		previous_vec = vec1;
	}
}

static void retargetArctoArc(bContext *C, RigGraph *rigg, RigArc *iarc, RigNode *inode_start)
{
	RetargetParam *p = MEM_callocN(sizeof(RetargetParam), "RetargetParam");
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	p->rigg = rigg;
	p->iarc = iarc;
	p->inode_start = inode_start;
	p->context = C;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	BLI_task_pool_push(rigg->task_pool, exec_retargetArctoArc, p, true, TASK_PRIORITY_HIGH);
}

void exec_retargetArctoArc(TaskPool *UNUSED(pool), void *taskdata, int UNUSED(threadid))
{
	RetargetParam *p = (RetargetParam *)taskdata;
	RigGraph *rigg = p->rigg;
	RigArc *iarc = p->iarc;
	bContext *C = p->context;
	RigNode *inode_start = p->inode_start;
	ReebArc *earc = iarc->link_mesh;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	if (BLI_listbase_is_single(&iarc->edges)) {
		RigEdge *edge = iarc->edges.first;

		if (testFlipArc(iarc, inode_start)) {
			repositionBone(C, rigg, edge, earc->tail->p, earc->head->p, earc->head->no);
		}
		else {
			repositionBone(C, rigg, edge, earc->head->p, earc->tail->p, earc->tail->no);
		}
	}
	else {
		RetargetMode mode = detectArcRetargetMode(iarc);
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		if (mode == RETARGET_AGGRESSIVE) {
			retargetArctoArcAggresive(C, rigg, iarc, inode_start);
		}
		else {
			retargetArctoArcLength(C, rigg, iarc, inode_start);
		}
	}
}

static void matchMultiResolutionNode(RigGraph *rigg, RigNode *inode, ReebNode *top_node)
{
	ReebNode *enode = top_node;
	ReebGraph *reebg = BIF_graphForMultiNode(rigg->link_mesh, enode);
	int ishape, eshape;
<<<<<<< HEAD
	
	ishape = BLI_subtreeShape((BGraph *)rigg, (BNode *)inode, NULL, 0) % SHAPE_LEVELS;
	eshape = BLI_subtreeShape((BGraph *)reebg, (BNode *)enode, NULL, 0) % SHAPE_LEVELS;
	
=======

	ishape = BLI_subtreeShape((BGraph *)rigg, (BNode *)inode, NULL, 0) % SHAPE_LEVELS;
	eshape = BLI_subtreeShape((BGraph *)reebg, (BNode *)enode, NULL, 0) % SHAPE_LEVELS;

>>>>>>> Initial commit
	inode->link_mesh = enode;

	while (ishape == eshape && enode->link_down) {
		inode->link_mesh = enode;

		enode = enode->link_down;
		reebg = BIF_graphForMultiNode(rigg->link_mesh, enode); /* replace with call to link_down once that exists */
		eshape = BLI_subtreeShape((BGraph *)reebg, (BNode *)enode, NULL, 0) % SHAPE_LEVELS;
	}
}

static void markMultiResolutionChildArc(ReebNode *end_enode, ReebNode *enode)
{
	int i;
<<<<<<< HEAD
	
	for (i = 0; i < enode->degree; i++) {
		ReebArc *earc = (ReebArc *)enode->arcs[i];
		
		if (earc->flag == ARC_FREE) {
			earc->flag = ARC_TAKEN;
			
=======

	for (i = 0; i < enode->degree; i++) {
		ReebArc *earc = (ReebArc *)enode->arcs[i];

		if (earc->flag == ARC_FREE) {
			earc->flag = ARC_TAKEN;

>>>>>>> Initial commit
			if (earc->tail->degree > 1 && earc->tail != end_enode) {
				markMultiResolutionChildArc(end_enode, earc->tail);
			}
			break;
		}
	}
}

static void markMultiResolutionArc(ReebArc *start_earc)
{
	if (start_earc->link_up) {
		ReebArc *earc;
		for (earc = start_earc->link_up; earc; earc = earc->link_up) {
			earc->flag = ARC_TAKEN;
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			if (earc->tail->index != start_earc->tail->index) {
				markMultiResolutionChildArc(earc->tail, earc->tail);
			}
		}
	}
}

static void matchMultiResolutionArc(RigGraph *rigg, RigNode *start_node, RigArc *next_iarc, ReebArc *next_earc)
{
	ReebNode *enode = next_earc->head;
	ReebGraph *reebg = BIF_graphForMultiNode(rigg->link_mesh, enode);
	int ishape, eshape;

	ishape = BLI_subtreeShape((BGraph *)rigg, (BNode *)start_node, (BArc *)next_iarc, 1) % SHAPE_LEVELS;
	eshape = BLI_subtreeShape((BGraph *)reebg, (BNode *)enode, (BArc *)next_earc, 1) % SHAPE_LEVELS;
<<<<<<< HEAD
	
	while (ishape != eshape && next_earc->link_up) {
		next_earc->flag = ARC_TAKEN; // mark previous as taken, to prevent backtrack on lower levels
		
=======

	while (ishape != eshape && next_earc->link_up) {
		next_earc->flag = ARC_TAKEN; // mark previous as taken, to prevent backtrack on lower levels

>>>>>>> Initial commit
		next_earc = next_earc->link_up;
		reebg = reebg->link_up;
		enode = next_earc->head;
		eshape = BLI_subtreeShape((BGraph *)reebg, (BNode *)enode, (BArc *)next_earc, 1) % SHAPE_LEVELS;
	}

	next_earc->flag = ARC_USED;
	next_iarc->link_mesh = next_earc;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* mark all higher levels as taken too */
	markMultiResolutionArc(next_earc);
//	while (next_earc->link_up)
//	{
//		next_earc = next_earc->link_up;
//		next_earc->flag = ARC_TAKEN;
//	}
}

static void matchMultiResolutionStartingNode(RigGraph *rigg, ReebGraph *reebg, RigNode *inode)
{
	ReebNode *enode;
	int ishape, eshape;
<<<<<<< HEAD
	
	enode = reebg->nodes.first;
	
	ishape = BLI_subtreeShape((BGraph *)rigg, (BNode *)inode, NULL, 0) % SHAPE_LEVELS;
	eshape = BLI_subtreeShape((BGraph *)rigg->link_mesh, (BNode *)enode, NULL, 0) % SHAPE_LEVELS;
	
	while (ishape != eshape && reebg->link_up) {
		reebg = reebg->link_up;
		
		enode = reebg->nodes.first;
		
=======

	enode = reebg->nodes.first;

	ishape = BLI_subtreeShape((BGraph *)rigg, (BNode *)inode, NULL, 0) % SHAPE_LEVELS;
	eshape = BLI_subtreeShape((BGraph *)rigg->link_mesh, (BNode *)enode, NULL, 0) % SHAPE_LEVELS;

	while (ishape != eshape && reebg->link_up) {
		reebg = reebg->link_up;

		enode = reebg->nodes.first;

>>>>>>> Initial commit
		eshape = BLI_subtreeShape((BGraph *)reebg, (BNode *)enode, NULL, 0) % SHAPE_LEVELS;
	}

	inode->link_mesh = enode;
}

static void findCorrespondingArc(RigGraph *rigg, RigArc *start_arc, RigNode *start_node, RigArc *next_iarc, int root)
{
	ReebNode *enode = start_node->link_mesh;
	ReebArc *next_earc;
	int symmetry_level = next_iarc->symmetry_level;
	int symmetry_group = next_iarc->symmetry_group;
	int symmetry_flag = next_iarc->symmetry_flag;
	int i;
<<<<<<< HEAD
	
	next_iarc->link_mesh = NULL;
		
=======

	next_iarc->link_mesh = NULL;

>>>>>>> Initial commit
//	if (root)
//	{
//		printf("-----------------------\n");
//		printf("MATCHING LIMB\n");
//		RIG_printArcBones(next_iarc);
//	}
<<<<<<< HEAD
	
	for (i = 0; i < enode->degree; i++) {
		next_earc = (ReebArc *)enode->arcs[i];
		
=======

	for (i = 0; i < enode->degree; i++) {
		next_earc = (ReebArc *)enode->arcs[i];

>>>>>>> Initial commit
//		if (next_earc->flag == ARC_FREE)
//		{
//			printf("candidate (level %i ?= %i) (flag %i ?= %i) (group %i ?= %i)\n",
//			symmetry_level, next_earc->symmetry_level,
<<<<<<< HEAD
//			symmetry_flag, next_earc->symmetry_flag, 
//			symmetry_group, next_earc->symmetry_flag);
//		}
		
=======
//			symmetry_flag, next_earc->symmetry_flag,
//			symmetry_group, next_earc->symmetry_flag);
//		}

>>>>>>> Initial commit
		if (next_earc->flag == ARC_FREE &&
		    next_earc->symmetry_flag == symmetry_flag &&
		    next_earc->symmetry_group == symmetry_group &&
		    next_earc->symmetry_level == symmetry_level)
		{
//			printf("CORRESPONDING ARC FOUND\n");
//			printf("flag %i -- level %i -- flag %i -- group %i\n", next_earc->flag, next_earc->symmetry_level, next_earc->symmetry_flag, next_earc->symmetry_group);
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			matchMultiResolutionArc(rigg, start_node, next_iarc, next_earc);
			break;
		}
	}
<<<<<<< HEAD
	
	/* not found, try at higher nodes (lower node might have filtered internal arcs, messing shape of tree */
	if (next_iarc->link_mesh == NULL) {
//		printf("NO CORRESPONDING ARC FOUND - GOING TO HIGHER LEVELS\n");
		
=======

	/* not found, try at higher nodes (lower node might have filtered internal arcs, messing shape of tree */
	if (next_iarc->link_mesh == NULL) {
//		printf("NO CORRESPONDING ARC FOUND - GOING TO HIGHER LEVELS\n");

>>>>>>> Initial commit
		if (enode->link_up) {
			start_node->link_mesh = enode->link_up;
			findCorrespondingArc(rigg, start_arc, start_node, next_iarc, 0);
		}
	}

	/* still not found, print debug info */
	if (root && next_iarc->link_mesh == NULL) {
		start_node->link_mesh = enode; /* linking back with root node */
<<<<<<< HEAD
		
//		printf("NO CORRESPONDING ARC FOUND\n");
//		RIG_printArcBones(next_iarc);
//		
//		printf("ON NODE %i, multilevel %i\n", enode->index, enode->multi_level);
//		
//		printf("LOOKING FOR\n");
//		printf("flag %i -- level %i -- flag %i -- group %i\n", ARC_FREE, symmetry_level, symmetry_flag, symmetry_group);
//		
=======

//		printf("NO CORRESPONDING ARC FOUND\n");
//		RIG_printArcBones(next_iarc);
//
//		printf("ON NODE %i, multilevel %i\n", enode->index, enode->multi_level);
//
//		printf("LOOKING FOR\n");
//		printf("flag %i -- level %i -- flag %i -- group %i\n", ARC_FREE, symmetry_level, symmetry_flag, symmetry_group);
//
>>>>>>> Initial commit
//		printf("CANDIDATES\n");
//		for (i = 0; i < enode->degree; i++)
//		{
//			next_earc = (ReebArc *)enode->arcs[i];
//			printf("flag %i -- level %i -- flag %i -- group %i\n", next_earc->flag, next_earc->symmetry_level, next_earc->symmetry_flag, next_earc->symmetry_group);
//		}
<<<<<<< HEAD
		
		/* Emergency matching */
		for (i = 0; i < enode->degree; i++) {
			next_earc = (ReebArc *)enode->arcs[i];
			
=======

		/* Emergency matching */
		for (i = 0; i < enode->degree; i++) {
			next_earc = (ReebArc *)enode->arcs[i];

>>>>>>> Initial commit
			if (next_earc->flag == ARC_FREE && next_earc->symmetry_level == symmetry_level) {
//				printf("USING:\n");
//				printf("flag %i -- level %i -- flag %i -- group %i\n", next_earc->flag, next_earc->symmetry_level, next_earc->symmetry_flag, next_earc->symmetry_group);
				matchMultiResolutionArc(rigg, start_node, next_iarc, next_earc);
				break;
			}
		}
	}

}

static void retargetSubgraph(bContext *C, RigGraph *rigg, RigArc *start_arc, RigNode *start_node)
{
	RigNode *inode = start_node;
	int i;

	/* no start arc on first node */
	if (start_arc) {
		ReebNode *enode = start_node->link_mesh;
		ReebArc *earc = start_arc->link_mesh;
<<<<<<< HEAD
		
		retargetArctoArc(C, rigg, start_arc, start_node);
		
		enode = BIF_otherNodeFromIndex(earc, enode);
		inode = (RigNode *)BLI_otherNode((BArc *)start_arc, (BNode *)inode);
	
		/* match with lowest node with correct shape */
		matchMultiResolutionNode(rigg, inode, enode);
	}
	
	for (i = 0; i < inode->degree; i++) {
		RigArc *next_iarc = (RigArc *)inode->arcs[i];
		
=======

		retargetArctoArc(C, rigg, start_arc, start_node);

		enode = BIF_otherNodeFromIndex(earc, enode);
		inode = (RigNode *)BLI_otherNode((BArc *)start_arc, (BNode *)inode);

		/* match with lowest node with correct shape */
		matchMultiResolutionNode(rigg, inode, enode);
	}

	for (i = 0; i < inode->degree; i++) {
		RigArc *next_iarc = (RigArc *)inode->arcs[i];

>>>>>>> Initial commit
		/* no back tracking */
		if (next_iarc != start_arc) {
			findCorrespondingArc(rigg, start_arc, inode, next_iarc, 1);
			if (next_iarc->link_mesh) {
				retargetSubgraph(C, rigg, next_iarc, inode);
			}
		}
	}
}

static void finishRetarget(RigGraph *rigg)
{
	BLI_task_pool_work_and_wait(rigg->task_pool);
}

static void adjustGraphs(bContext *C, RigGraph *rigg)
{
	bArmature *arm = rigg->ob->data;
	RigArc *arc;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	for (arc = rigg->arcs.first; arc; arc = arc->next) {
		if (arc->link_mesh) {
			retargetArctoArc(C, rigg, arc, arc->head);
		}
	}

	finishRetarget(rigg);

	/* Turn the list into an armature */
	arm->edbo = rigg->editbones;
	ED_armature_from_edit(arm);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	ED_undo_push(C, "Retarget Skeleton");
}

static void retargetGraphs(bContext *C, RigGraph *rigg)
{
	bArmature *arm = rigg->ob->data;
	ReebGraph *reebg = rigg->link_mesh;
	RigNode *inode;
<<<<<<< HEAD
	
	/* flag all ReebArcs as free */
	BIF_flagMultiArcs(reebg, ARC_FREE);
	
	/* return to first level */
	inode = rigg->head;
	
	matchMultiResolutionStartingNode(rigg, reebg, inode);

	retargetSubgraph(C, rigg, NULL, inode);
	
	//generateMissingArcs(rigg);
	
=======

	/* flag all ReebArcs as free */
	BIF_flagMultiArcs(reebg, ARC_FREE);

	/* return to first level */
	inode = rigg->head;

	matchMultiResolutionStartingNode(rigg, reebg, inode);

	retargetSubgraph(C, rigg, NULL, inode);

	//generateMissingArcs(rigg);

>>>>>>> Initial commit
	finishRetarget(rigg);

	/* Turn the list into an armature */
	arm->edbo = rigg->editbones;
	ED_armature_from_edit(arm);
}

const char *RIG_nameBone(RigGraph *rg, int arc_index, int bone_index)
{
	RigArc *arc = BLI_findlink(&rg->arcs, arc_index);
	RigEdge *iedge;

	if (arc == NULL) {
		return "None";
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	if (bone_index == BLI_listbase_count(&arc->edges)) {
		return "Last joint";
	}

	iedge = BLI_findlink(&arc->edges, bone_index);
<<<<<<< HEAD
	
	if (iedge == NULL) {
		return "Done";
	}
	
	if (iedge->bone == NULL) {
		return "Bone offset";
	}
	
=======

	if (iedge == NULL) {
		return "Done";
	}

	if (iedge->bone == NULL) {
		return "Bone offset";
	}

>>>>>>> Initial commit
	return iedge->bone->name;
}

int RIG_nbJoints(RigGraph *rg)
{
	RigArc *arc;
	int total = 0;
<<<<<<< HEAD
	
	total += BLI_listbase_count(&rg->nodes);
	
	for (arc = rg->arcs.first; arc; arc = arc->next) {
		total += BLI_listbase_count(&arc->edges) - 1; /* -1 because end nodes are already counted */
	}
	
=======

	total += BLI_listbase_count(&rg->nodes);

	for (arc = rg->arcs.first; arc; arc = arc->next) {
		total += BLI_listbase_count(&arc->edges) - 1; /* -1 because end nodes are already counted */
	}

>>>>>>> Initial commit
	return total;
}

static void BIF_freeRetarget(void)
{
	if (GLOBAL_RIGG) {
		RIG_freeRigGraph((BGraph *)GLOBAL_RIGG);
		GLOBAL_RIGG = NULL;
	}
}

void BIF_retargetArmature(bContext *C)
{
	ReebGraph *reebg;
	double start_time, end_time;
	double gstart_time, gend_time;
	double reeb_time, rig_time = 0.0, retarget_time = 0.0, total_time;
<<<<<<< HEAD
	
	gstart_time = start_time = PIL_check_seconds_timer();
	
	reebg = BIF_ReebGraphMultiFromEditMesh(C);
	
	end_time = PIL_check_seconds_timer();
	reeb_time = end_time - start_time;
	
=======

	gstart_time = start_time = PIL_check_seconds_timer();

	reebg = BIF_ReebGraphMultiFromEditMesh(C);

	end_time = PIL_check_seconds_timer();
	reeb_time = end_time - start_time;

>>>>>>> Initial commit
	printf("Reeb Graph created\n");

	CTX_DATA_BEGIN (C, Base *, base, selected_editable_bases)
	{
		Object *ob = base->object;

		if (ob->type == OB_ARMATURE) {
			RigGraph *rigg;
			bArmature *arm;

			arm = ob->data;
<<<<<<< HEAD
		
			/* Put the armature into editmode */
			
		
			start_time = PIL_check_seconds_timer();

			rigg = RIG_graphFromArmature(C, ob, arm);
			
=======

			/* Put the armature into editmode */


			start_time = PIL_check_seconds_timer();

			rigg = RIG_graphFromArmature(C, ob, arm);

>>>>>>> Initial commit
			end_time = PIL_check_seconds_timer();
			rig_time = end_time - start_time;

			printf("Armature graph created\n");
<<<<<<< HEAD
	
			//RIG_printGraph(rigg);
			
			rigg->link_mesh = reebg;
			
			printf("retargetting %s\n", ob->id.name);
			
			start_time = PIL_check_seconds_timer();

			retargetGraphs(C, rigg);
			
=======

			//RIG_printGraph(rigg);

			rigg->link_mesh = reebg;

			printf("retargetting %s\n", ob->id.name);

			start_time = PIL_check_seconds_timer();

			retargetGraphs(C, rigg);

>>>>>>> Initial commit
			end_time = PIL_check_seconds_timer();
			retarget_time = end_time - start_time;

			BIF_freeRetarget();
<<<<<<< HEAD
			
			GLOBAL_RIGG = rigg;
			
=======

			GLOBAL_RIGG = rigg;

>>>>>>> Initial commit
			break; /* only one armature at a time */
		}
	}
	CTX_DATA_END;

<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	gend_time = PIL_check_seconds_timer();

	total_time = gend_time - gstart_time;

	printf("-----------\n");
	printf("runtime: \t%.3f\n", total_time);
	printf("reeb: \t\t%.3f (%.1f%%)\n", reeb_time, reeb_time / total_time * 100);
	printf("rig: \t\t%.3f (%.1f%%)\n", rig_time, rig_time / total_time * 100);
	printf("retarget: \t%.3f (%.1f%%)\n", retarget_time, retarget_time / total_time * 100);
	printf("-----------\n");
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	ED_undo_push(C, "Retarget Skeleton");

	// XXX
//	allqueue(REDRAWVIEW3D, 0);
}

void BIF_retargetArc(bContext *C, ReebArc *earc, RigGraph *template_rigg)
{
	Object *obedit = CTX_data_edit_object(C);
	Scene *scene = CTX_data_scene(C);
	bArmature *armedit = obedit->data;
	Object *ob;
	RigGraph *rigg;
	RigArc *iarc;
	char *side_string = scene->toolsettings->skgen_side_string;
	char *num_string = scene->toolsettings->skgen_num_string;
	int free_template = 0;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	if (template_rigg) {
		ob = template_rigg->ob;
	}
	else {
		free_template = 1;
		ob = obedit;
		template_rigg = armatureSelectedToGraph(C, ob, ob->data);
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	if (BLI_listbase_is_empty(&template_rigg->arcs)) {
//		XXX
//		error("No Template and no deforming bones selected");
		return;
	}
<<<<<<< HEAD
	
	rigg = cloneRigGraph(template_rigg, armedit->edbo, obedit, side_string, num_string);
	
	iarc = rigg->arcs.first;
	
	iarc->link_mesh = earc;
	iarc->head->link_mesh = earc->head;
	iarc->tail->link_mesh = earc->tail;
	
	retargetArctoArc(C, rigg, iarc, iarc->head);
	
	finishRetarget(rigg);
	
=======

	rigg = cloneRigGraph(template_rigg, armedit->edbo, obedit, side_string, num_string);

	iarc = rigg->arcs.first;

	iarc->link_mesh = earc;
	iarc->head->link_mesh = earc->head;
	iarc->tail->link_mesh = earc->tail;

	retargetArctoArc(C, rigg, iarc, iarc->head);

	finishRetarget(rigg);

>>>>>>> Initial commit
	/* free template if it comes from the edit armature */
	if (free_template) {
		RIG_freeRigGraph((BGraph *)template_rigg);
	}
	RIG_freeRigGraph((BGraph *)rigg);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	ED_armature_validate_active(armedit);

//	XXX
//	allqueue(REDRAWVIEW3D, 0);
}

void BIF_adjustRetarget(bContext *C)
{
	if (GLOBAL_RIGG) {
		adjustGraphs(C, GLOBAL_RIGG);
	}
}