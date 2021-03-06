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
 * Contributor(s): Full recode, Ton Roosendaal, Crete 2005
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file DNA_armature_types.h
 *  \ingroup DNA
 */

#ifndef __DNA_ARMATURE_TYPES_H__
#define __DNA_ARMATURE_TYPES_H__

#include "DNA_defs.h"
#include "DNA_listBase.h"
#include "DNA_ID.h"

struct AnimData;

/* this system works on different transformation space levels;
 *
 * 1) Bone Space;      with each Bone having own (0,0,0) origin
 * 2) Armature Space;  the rest position, in Object space, Bones Spaces are applied hierarchical
 * 3) Pose Space;      the animation position, in Object space
 * 4) World Space;     Object matrix applied to Pose or Armature space
 *
 */

#define AE_BONE     (1 << 0)
#define AE_MUSCLE   (1 << 1)

typedef struct ArmatureElement {
    // Common elements to both Bone and Muscle
    struct ArmatureElement *next, *prev;
    IDProperty             *prop;
    struct ArmatureElement *parent;

    // Custom elements go here
    // Like the dist or weight values of a Bone struct
    // Or the Bone pointers of a Muscle struct
    void                   *data;

    ListBase                childbase;
    char                    name[64];

    int                     type;

    float                   roll;
    float                   head[3];
    float                   tail[3];
    float                   AE_mat[3][3];

    int                     flag;

    float                   arm_head[3];
    float                   arm_tail[3];
    float                   arm_mat[4][4];
    float                   arm_roll;

    float                   rad_head, rad_tail;

    float                   xwidth, length, zwidth;

    int                     layer;
    short                   segments;

    char                    pad[2];
} ArmatureElement;

typedef struct BoneData {
    float   dist, weight;
    float   ease1, ease2;

    float   size[3];
} BoneData;

typedef struct MuscleData {
    struct ArmatureElement *start, *end;
} MuscleData;

typedef struct Bone {
	struct Bone *next, *prev;    /*  Next/prev elements within this list */
	IDProperty  *prop;           /* User-Defined Properties on this Bone */
	struct Bone *parent;         /*  Parent (ik parent if appropriate flag is set */
	ListBase     childbase;      /*  Children    */
	char         name[64];       /*  Name of the bone - must be unique within the armature, MAXBONENAME */

	float        roll;           /*  roll is input for editmode, length calculated */
	float        head[3];
	float        tail[3];        /*  head/tail and roll in Bone Space    */
	float        bone_mat[3][3]; /*  rotation derived from head/tail/roll */

	int          flag;

	float        arm_head[3];
	float        arm_tail[3];    /*  head/tail in Armature Space (rest pos) */
	float        arm_mat[4][4];  /*  matrix: (bonemat(b)+head(b))*arm_mat(b-1), rest pos*/
	float        arm_roll;       /* roll in Armature Space (rest pos) */

	float        dist, weight;           /*  dist, weight: for non-deformgroup deforms */
	float        xwidth, length, zwidth; /*  width: for block bones. keep in this order, transform! */
	float        ease1, ease2;           /*  length of bezier handles */
	float        rad_head, rad_tail;     /* radius for head/tail sphere, defining deform as well, parent->rad_tip overrides rad_head */

	float        size[3];        /*  patch for upward compat, UNUSED! */
	int          layer;          /* layers that bone appears on */
	short        segments;       /*  for B-bones */
	short        pad[1];
} Bone;

//typedef struct Muscle {
//	struct Muscle  *next, *prev; /* Next/prev elements within this list */
//	IDProperty     *prop; /* User-defined properties */
//	struct Muscle  *parent;
//	struct Bone    *start, *end; /* Bones the muscle is connected to */
//	ListBase        childbase;
//	char            name[64]; /* Name of the muscle */
//
//	float           roll;
//	float           head[3]; /* Offset from start bone head */
//	float           tail[3]; /* Offset from end bone head */
//	float           muscle_mat[3][3]; /* Rotation derived from head/tail */
//
//	int             flag;
//
//	float           arm_head[3];
//	float           arm_tail[3];
//	float           arm_mat[4][4];
//	float           arm_roll;
//
//	float           length;
//
//	float           rad_head, rad_tail;
//
//	int             layer; /* Layer that muscle appears on */
//	short           segments;
//	char            pad[6];
//} Muscle;

typedef struct bArmature {
	ID          id;
	struct AnimData *adt;
    /* Kept around until a better solution is found */
	ListBase    bonebase;               /* Contains all the bones when writing an armature out to file */

    ListBase    elementbase;
	ListBase    chainbase;
    ListBase   *edel;

	/* active bones should work like active object where possible
	 * - active and selection are unrelated
	 * - active & hidden is not allowed
	 * - from the user perspective active == last selected
	 * - active should be ignored when not visible (hidden layer) */

	ArmatureElement *act_element;
	struct EditArmatureElement *act_edelement;

	void       *sketch;                 /* sketch struct for etch-a-ton */

	int         flag;
	int         drawtype;
	int         gevertdeformer;         /* how vertex deformation is handled in the ge */
	int         pad;
	short       deformflag;
	short       pathflag;

	unsigned int layer_used;             /* for UI, to show which layers are there */
	unsigned int layer, layer_protected; /* for buttons to work, both variables in this order together */

// XXX deprecated... old animaton system (armature only viz) ---
	short       ghostep, ghostsize;     /* number of frames to ghosts to show, and step between them  */
	short       ghosttype, pathsize;        /* ghost drawing options and number of frames between points of path */
	int         ghostsf, ghostef;       /* start and end frames of ghost-drawing range */
	int         pathsf, pathef;         /* start and end frames of path-calculation range for all bones */
	int         pathbc, pathac;         /* number of frames before/after current frame of path-calculation for all bones  */
// XXX end of deprecated code ----------------------------------
} bArmature;

/* armature->flag */
/* don't use bit 7, was saved in files to disable stuff */
typedef enum eArmature_Flag {
	ARM_RESTPOS         = (1<<0),
	ARM_DRAWXRAY        = (1<<1),   /* XRAY is here only for backwards converting */
	ARM_DRAWAXES        = (1<<2),
	ARM_DRAWNAMES       = (1<<3),
	ARM_POSEMODE        = (1<<4),
	ARM_EDITMODE        = (1<<5),
	ARM_DELAYDEFORM     = (1<<6),
	ARM_DONT_USE        = (1<<7),
	ARM_MIRROR_EDIT     = (1<<8),
	ARM_AUTO_IK         = (1<<9),
	ARM_NO_CUSTOM       = (1<<10),  /* made option negative, for backwards compat */
	ARM_COL_CUSTOM      = (1<<11),  /* draw custom colors  */
	ARM_GHOST_ONLYSEL   = (1<<12),  /* when ghosting, only show selected bones (this should belong to ghostflag instead) */ /* XXX deprecated */
	ARM_DS_EXPAND       = (1<<13),  /* dopesheet channel is expanded */
	ARM_HAS_VIZ_DEPS    = (1<<14),  /* other objects are used for visualizing various states (hack for efficient updates) */
} eArmature_Flag;

/* armature->drawtype */
typedef enum eArmature_Drawtype {
	ARM_OCTA = 0,
	ARM_LINE = 1,
	ARM_B_BONE = 2,
	ARM_ENVELOPE = 3,
	ARM_WIRE = 4
} eArmature_Drawtype;

/* armature->gevertdeformer */
typedef enum eArmature_VertDeformer {
	ARM_VDEF_BLENDER = 0,
	ARM_VDEF_BGE_CPU = 1
} eArmature_VertDeformer;

/* armature->deformflag */
typedef enum eArmature_DeformFlag {
	ARM_DEF_VGROUP			= (1<<0),
	ARM_DEF_ENVELOPE		= (1<<1),
	ARM_DEF_QUATERNION		= (1<<2),
	ARM_DEF_B_BONE_REST		= (1<<3),	/* deprecated */
	ARM_DEF_INVERT_VGROUP	= (1<<4)
} eArmature_DeformFlag;

#if (DNA_DEPRECATED_GCC_POISON == 1)
#pragma GCC poison ARM_DEF_B_BONE_REST
#endif

/* armature->pathflag */
// XXX deprecated... old animation system (armature only viz)
typedef enum eArmature_PathFlag {
	ARM_PATH_FNUMS		= (1<<0),
	ARM_PATH_KFRAS		= (1<<1),
	ARM_PATH_HEADS		= (1<<2),
	ARM_PATH_ACFRA		= (1<<3),
	ARM_PATH_KFNOS		= (1<<4)
} eArmature_PathFlag;

#if (DNA_DEPRECATED_GCC_POISON == 1)
#pragma GCC poison ARM_PATH_FNUMS ARM_PATH_KFRAS ARM_PATH_HEADS ARM_PATH_ACFRA ARM_PATH_KFNOS
#endif

/* armature->ghosttype */
// XXX deprecated... old animation system (armature only viz)
typedef enum eArmature_GhostType {
	ARM_GHOST_CUR = 0,
	ARM_GHOST_RANGE = 1,
	ARM_GHOST_KEYS = 2
} eArmature_GhostType;

/* bone->flag */
//typedef enum eBone_Flag {
//	BONE_SELECTED               = (1 << 0),
//	BONE_ROOTSEL                = (1 << 1),
//	BONE_TIPSEL                 = (1 << 2),
//	BONE_TRANSFORM              = (1 << 3),   /* Used instead of BONE_SELECTED during transform */
//	BONE_CONNECTED              = (1 << 4),   /* when bone has a parent, connect head of bone to parent's tail*/
//	/* 32 used to be quatrot, was always set in files, do not reuse unless you clear it always */
//	BONE_HIDDEN_P               = (1 << 6),   /* hidden Bones when drawing PoseChannels */
//	BONE_DONE                   = (1 << 7),   /* For detecting cyclic dependencies */
//	BONE_DRAW_ACTIVE            = (1 << 8),   /* active is on mouse clicks only - deprecated, ONLY USE FOR DRAWING */
//	BONE_HINGE                  = (1 << 9),   /* No parent rotation or scale */
//	BONE_HIDDEN_A               = (1 << 10),  /* hidden Bones when drawing Armature Editmode */
//	BONE_MULT_VG_ENV            = (1 << 11),  /* multiplies vgroup with envelope */
//	BONE_NO_DEFORM              = (1 << 12),  /* bone doesn't deform geometry */
//	BONE_UNKEYED                = (1 << 13),  /* set to prevent destruction of its unkeyframed pose (after transform) */
//	BONE_HINGE_CHILD_TRANSFORM  = (1 << 14),  /* set to prevent hinge child bones from influencing the transform center */
//	BONE_NO_SCALE               = (1 << 15),  /* No parent scale */
//	BONE_HIDDEN_PG              = (1 << 16),  /* hidden bone when drawing PoseChannels (for ghost drawing) */
//	BONE_DRAWWIRE               = (1 << 17),  /* bone should be drawn as OB_WIRE, regardless of draw-types of view+armature */
//	BONE_NO_CYCLICOFFSET        = (1 << 18),  /* when no parent, bone will not get cyclic offset */
//	BONE_EDITMODE_LOCKED        = (1 << 19),  /* bone transforms are locked in EditMode */
//	BONE_TRANSFORM_CHILD        = (1 << 20),  /* Indicates that a parent is also being transformed */
//	BONE_UNSELECTABLE           = (1 << 21),  /* bone cannot be selected */
//	BONE_NO_LOCAL_LOCATION      = (1 << 22),  /* bone location is in armature space */
//	BONE_RELATIVE_PARENTING     = (1 << 23)   /* object child will use relative transform (like deform) */
//
//} eBone_Flag;

typedef enum eElement_Flag {
    ELEMENT_SELECTED                = (1 << 0),
    ELEMENT_ROOTSEL                 = (1 << 1),
    ELEMENT_TIPSEL                  = (1 << 2),
    ELEMENT_TRANSFORM               = (1 << 3),  /* Used instead of BONE_SELECTED during transform */
    ELEMENT_CONNECTED               = (1 << 4),  /* when bone has a parent, connect head of bone to parent's tail*/
    /* 32 used to be quatrot, was always set in files, do not reuse unless you clear it always */
    ELEMENT_HIDDEN_P                = (1 << 6),  /* hidden Bones when drawing PoseChannels */
    ELEMENT_DONE                    = (1 << 7),  /* For detecting cyclic dependencies */
    ELEMENT_DRAW_ACTIVE             = (1 << 8),  /* active is on mouse clicks only - deprecated, ONLY USE FOR DRAWING */
    ELEMENT_HINGE                   = (1 << 9),  /* No parent rotation or scale */
    ELEMENT_HIDDEN_A                = (1 << 10), /* hidden Bones when drawing Armature Editmode */
    ELEMENT_MULT_VG_ENV             = (1 << 11), /* multiplies vgroup with envelope */
    ELEMENT_NO_DEFORM               = (1 << 12), /* bone doesn't deform geometry */
    ELEMENT_UNKEYED                 = (1 << 13), /* set to prevent destruction of its unkeyframed pose (after transform) */
    ELEMENT_HINGE_CHILD_TRANSFORM   = (1 << 14), /* set to prevent hinge child bones from influencing the transform center */
    ELEMENT_NO_SCALE                = (1 << 15), /* No parent scale */
    ELEMENT_HIDDEN_PG               = (1 << 16), /* hidden bone when drawing PoseChannels (for ghost drawing) */
    ELEMENT_DRAWWIRE                = (1 << 17), /* bone should be drawn as OB_WIRE, regardless of draw-types of view+armature */
    ELEMENT_NO_CYCLICOFFSET         = (1 << 18), /* when no parent, bone will not get cyclic offset */
    ELEMENT_EDITMODE_LOCKED         = (1 << 19), /* bone transforms are locked in EditMode */
    ELEMENT_TRANSFORM_CHILD         = (1 << 20), /* Indicates that a parent is also being transformed */
    ELEMENT_UNSELECTABLE            = (1 << 21), /* bone cannot be selected */
    ELEMENT_NO_LOCAL_LOCATION       = (1 << 22), /* bone location is in armature space */
    ELEMENT_RELATIVE_PARENTING      = (1 << 23)  /* object child will use relative transform (like deform) */
} eElement_Flag;

#define MAXBONENAME 64
#define MAXMUSCLENAME 64

#endif
