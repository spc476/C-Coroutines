/***************************************************************************
*
* Copyright 2017 by Sean Conner.
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, see <http://www.gnu.org/licenses/>.
*
* Comments, questions and criticisms can be sent to: sean@conman.org
*
*************************************************************************/

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "coroutine.h"

typedef struct tree
{
  struct tree *left;
  struct tree *right;
  int          height;
} tree__s;

extern tree__s *tree_insert   (tree__s *restrict,tree__s *restrict,int (*)(const void *,const void *));
extern tree__s *tree_find     (tree__s *,const void *,int (*)(const void *,const void *));
extern tree__s *tree_remove   (tree__s *,const void *,int (*)(const void *,const void *),tree__s **);

static int      tree_delta    (tree__s *);
static tree__s  *tree_rotl    (tree__s *);
static tree__s  *tree_rotr    (tree__s *);
static tree__s  *tree_balance (tree__s *);

struct word
{
  tree__s  node;
  char    *word;
};

/*************************************************************************/

static const struct word *c_tree_to_word(const tree__s *tree)
{
  return (const struct word *)((const char *)tree - offsetof(struct word,node));
}

/*************************************************************************/

static struct word *tree_to_word(tree__s *tree)
{
  return (struct word *)((char *)tree - offsetof(struct word,node));
}

/*************************************************************************/

static int wordcmp(const void *needle,const void *haystack)
{
  const struct word *left  = c_tree_to_word(needle);
  const struct word *right = c_tree_to_word(haystack);
  
  return strcmp(left->word,right->word);
}

/*************************************************************************/

static tree__s *readlist(const char *fname)
{
  tree__s     *tree = NULL;
  struct word *word;
  FILE        *fp;
  char         buffer[BUFSIZ];
  size_t       len;
  
  fp = (fname != NULL) ? fopen(fname,"r") : stdin;

  while(fgets(buffer,sizeof(buffer),fp))
  {
    
    word       = calloc(1,sizeof(struct word));
    len        = strlen(buffer) + 1;
    word->word = malloc(len + 1);
    memcpy(word->word,buffer,len);
    tree = tree_insert(tree,&word->node,wordcmp);
  }
  
  if (fp != stdin) fclose(fp);
  return tree;
}

/*************************************************************************/

static void freelist(tree__s *tree)
{
  struct word *word;
  
  if (tree->left) freelist(tree->left);
  if (tree->right) freelist(tree->right);
  word = tree_to_word(tree);
  free(word->word);
  free(tree);
}

/*************************************************************************/

static uintptr_t word_iter(coroutine__s *self,uintptr_t utree)
{
  const tree__s     *tree = (const tree__s *)utree;
  const struct word *word;
  
  if (tree->left) word_iter(self,(uintptr_t)tree->left);
  
  word = c_tree_to_word(tree);
  coroutine_yield(self,(uintptr_t)word->word);
  
  if (tree->right) word_iter(self,(uintptr_t)tree->right);
  
  return 0;
}

/*************************************************************************/

static const char *next_word(coroutine__s **pco,tree__s *tree)
{
  uintptr_t item;
  
  if (*pco == NULL)
    coroutine_create(pco,0,word_iter);
  
  item = coroutine_yield(*pco,(uintptr_t)tree);

  if (item == 0)
  {
    coroutine_free(*pco);
    *pco = NULL;
  }
  
  return (const char *)item;
}

/*************************************************************************/

static void dumpcomem(coroutine__s *co)
{
  FILE *fp = fopen("/tmp/dump.bin","w");
  fwrite(co->base,1,co->size,fp);
  fclose(fp);
}

/*************************************************************************/

int main(int argc,char *argv[])
{
  tree__s      *list = NULL;
  struct word  *word = NULL;
  coroutine__s *co   = NULL;
  uintptr_t     item;
  const char   *w;
  const char   *fname;
  
  fname = (argc > 1) ? argv[1] : NULL;
  list  = readlist(fname);
  word  = tree_to_word(list);
  
  coroutine_create(&co,0,word_iter);
  while((item = coroutine_yield(co,(uintptr_t)list)) != 0)
    printf("%s",(const char *)item);
  dumpcomem(co);
  coroutine_free(co);
  co = NULL;
   
  printf("\n---\n\n");
  
  while((w = next_word(&co,list)) != NULL)
    printf("%s",w);
    
  freelist(list);
  
  return EXIT_SUCCESS;
}

/*************************************************************************
*
* AVL TREE IMPLEMENTATION
*
**************************************************************************/

static int tree_delta(tree__s *self)
{
  return (self->left  ? self->left->height  : 0)
       - (self->right ? self->right->height : 0);
}

/*************************************************************************/

static tree__s *tree_rotl(tree__s *self)
{
  tree__s *r  = self->right;
  self->right = r->left;
  r->left     = tree_balance(self);
  return tree_balance(r);
}

/*************************************************************************/

static tree__s *tree_rotr(tree__s *self)
{
  tree__s *l = self->left;
  self->left = l->right;
  l->right   = tree_balance(self);
  return tree_balance(l);
}

/*************************************************************************/

static tree__s *tree_balance(tree__s *self)
{
  int delta = tree_delta(self);
  
  if (delta < -1)
  {
    if (tree_delta(self->right) > 0)
      self->right = tree_rotr(self->right);
    return tree_rotl(self);
  }
  else if (delta > 1)
  {
    if (tree_delta(self->left) < 0)
      self->left = tree_rotl(self->left);
    return tree_rotr(self);
  }
  
  self->height = 0;
  
  if (self->left && (self->left->height > self->height))
    self->height = self->left->height;
    
  if (self->right && (self->right->height > self->height))
    self->height = self->right->height;
    
  self->height += 1;
  return self;
}

/*************************************************************************/

static tree__s *tree_move_right(tree__s *restrict self,tree__s *restrict rhs)
{
  if (self == NULL)
    return rhs;
    
  self->right = tree_move_right(self->right,rhs);
  return tree_balance(self);
}

/*************************************************************************/

tree__s *tree_insert(
        tree__s *restrict self,
        tree__s *restrict item,
        int (*compare)(const void *,const void *)
)
{
  int rc;
  
  if (self == NULL)
    return item;
    
  rc = (*compare)(item,self);
  
  if (rc < 0)
    self->left = tree_insert(self->left,item,compare);
  else
    self->right = tree_insert(self->right,item,compare);
    
  return tree_balance(self);
}

/*************************************************************************/

tree__s *tree_find(
        tree__s    *self,
        const void *item,
        int (*compare)(const void *,const void *)
)
{
  int rc;
  
  if (self == NULL)
    return NULL;
    
  rc = (*compare)(item,self);
  
  if (rc < 0)
    return tree_find(self->left,item,compare);
  else if (rc == 0)
    return self;
  else
    return tree_find(self->right,item,compare);
}

/*************************************************************************/

tree__s *tree_remove(
        tree__s     *self,
        const void  *item,
        int        (*compare)(const void *,const void *),
        tree__s    **remove
)
{
  int rc;
  
  if (self == NULL)
    return NULL;
    
  rc = (*compare)(item,self);
  
  if (rc < 0)
    self->left = tree_remove(self->left,item,compare,remove);
  else if (rc == 0)
  {
    if (remove != NULL)
      *remove = self;
      
    tree__s *tmp = tree_move_right(self->left,self->right);
    return tmp;
  }
  else
    self->right = tree_remove(self->right,item,compare,remove);
    
  return tree_balance(self);
}

/*************************************************************************/

