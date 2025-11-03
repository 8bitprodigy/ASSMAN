#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "assman.h"


static void AssNode_free_siblings(AssNode *);
static void AssNode_free(AssNode *);

typedef struct
Asset
{
	void         *data;
	AssReleaseFn  Release;
	void         *release_data;
	size_t        ref_count;
}
Asset;

typedef struct
AssNode
{
	struct AssNode 
					*parent,
					*child,
					*prev,
					*next;
	Asset           *asset;
	char             fragment[];
}
AssNode;

struct
AssMan
{
	AssNode *root;
};


static size_t
longest_common_prefix(const char *a, const char *b)
{
	size_t i = 0;
	while (
		a[i] && b[i] 
		&& a[i] == b[i]
	) i++;

	return i;
}


static Asset *
Asset_new(void *data, AssReleaseFn releaser, void *release_data)
{
	Asset *asset        = malloc(sizeof(Asset));
	asset->data         = data;
	asset->ref_count    = 1;
	asset->Release      = releaser;
	asset->release_data = release_data;

	return asset;
}

void
Asset_free(Asset *self)
{
	self->Release(self->data, self->release_data);
	free(self);
}


static AssNode *
AssNode_new(const char *fragment, Asset *asset)
{
	size_t   length = strlen(fragment);
	AssNode *node   = malloc( sizeof(AssNode) + length + 1);

    node->parent = NULL;
	node->child  = NULL;
    node->prev   = NULL;
	node->next   = NULL;
	node->asset  = asset;

	strcpy(node->fragment, fragment);
	return node;
}

static void
AssNode_free_siblings(AssNode *first)
{
    while (first) {
        AssNode *next = first->next;
        AssNode_free(first);
        first = next;
    }
}

static void
AssNode_free(AssNode *self)
{
	if (!self) return;
	
	AssNode_free_siblings(self->child);

	if (self->asset) {
		Asset_free(self->asset);
	}
	
	free(self);
}

static AssNode *
AssNode_walk(AssNode *self, const char *fragment)
{
	while (self) {
		size_t 
			match    = longest_common_prefix(self->fragment, fragment),
			frag_len = strlen(self->fragment);

		if (match == 0) {
			/* No common prefix -- try sibling */
			self = self->next;
			continue;
		}

		/* Partial match, node longer than path */
		if (match < frag_len) return NULL; 

		fragment += match; /* Exact match */
		if (*fragment == '\0') return self;

		self = self->child;
	}

	return NULL;
}

static AssNode *
AssNode_insert(AssNode **self, const char *path, Asset *asset)
{
	AssNode *node = *self;

	if (!node) {
		*self = AssNode_new(path, asset);
		return *self;
	}

	size_t
		match    = longest_common_prefix(node->fragment, path),
		frag_len = strlen(node->fragment),
		path_len = strlen(path);

	if (match == 0) {
		/* No common prefix -- try sibling */
		AssNode *result = AssNode_insert(&node->next, path, asset);
		if (node->next && !node->next->prev)
			node->next->prev = node;
		return result;
	}

	if (match < frag_len) {
		/* Split node -- shortest first. */
		AssNode *child = AssNode_new(node->fragment + match, node->asset);
		child->child = node->child;

		if (child->child)
			child->child->parent = child;

		node->fragment[match] = '\0';
		node->child           = child;
		child->parent         = node;
		node->asset           = NULL;

		if (match == path_len) {
			/* New asset ends here */
			node->asset = asset;
			return node;
		}
		else {
			AssNode *new_node = AssNode_new(path + match, asset);
			new_node->next    = node->child;
			new_node->parent  = node;
			
			node->child->prev = new_node;
			node->child       = new_node;
			return new_node;
		}
	}

	if (match == path_len) {
		/* Exact match -- assign asset */
		node->asset = asset;
		return node;
	}

	/* Full match of this fragment, Continue with recursive insertion */
	AssNode *result = AssNode_insert(&node->child, path + match, asset);
	if (node->child && !node->child->parent)
		node->child->parent = node;
		
	return result;
}

static void
AssNode_unlink(AssNode *node, AssMan *assman)
{
    if (!node) return;
    
    if (node->prev) {
        node->prev->next = node->next;
    } else if (node->parent) {
        node->parent->child = node->next;
    } else {
        assman->root = node->next;
    }
    
    if (node->next) {
        node->next->prev = node->prev;
    }
}


/******************
	A S S M A N
******************/
AssMan *
AssMan_new(void)
{
	AssMan *assman = malloc(sizeof(AssMan));
	if (!assman) return NULL;

	assman->root = NULL;

	return assman;
}

void
AssMan_free(AssMan *assman)
{
	AssMan_clear(assman);
	free(assman);
}

void *
AssMan_load(
	AssMan       *assman,
	const char   *path, 
	AssLoaderFn   loader, 
	void         *load_data, 
	AssReleaseFn  releaser, 
	void         *release_data
)
{
	if (!assman->root) {
		Asset *asset     = Asset_new(
				loader(path, load_data),
				releaser,
				release_data
			);
		assman->root = AssNode_new(path, asset);
		return asset->data;
	}

	AssNode *node = AssNode_walk(assman->root, path);
	if (node && node->asset) {
		/* Already loaded; increment ref_count */
		node->asset->ref_count++;
		return node->asset->data;
	}

	/* Node does not exist -- create new asset */
	Asset *asset = Asset_new(
			loader(path, load_data),
			releaser,
			release_data
		);
	AssNode_insert(&assman->root, path, asset);
	return asset->data;
}

void
AssMan_release(AssMan *assman, const char *path)
{
	AssNode *node = AssNode_walk(assman->root, path);

	if (!node) return;
	if (!node->asset) return;

	if (1 < node->asset->ref_count) {
		node->asset->ref_count--;
		return;
	}
	
	Asset_free(node->asset);
	node->asset = NULL;

	if (node->child) return;
	
	AssNode_unlink(node, assman);
	free(node);
}

void
AssMan_clear(AssMan *assman)
{
	AssNode_free(assman->root);
	assman->root       = NULL;
}
