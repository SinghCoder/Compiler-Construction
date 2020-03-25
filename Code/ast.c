#include "ast.h"
#include "astDef.h"
#include "treeADT.h"
#include <string.h>

void print_symbol(tree_node *temp) {
  if (temp->sym.is_terminal == true) {
    printf("%s\n", terminal_string[temp->sym.t]);
  } else {
    printf("%s\n", non_terminal_string[temp->sym.nt]);
  }
}

bool ends_in_dash(char *label) {
  int len = strlen(label);
  // printf("size is %d\n", len);
  if (len >= 4) {
    char *last_four = &label[len - 4];
    if (strcmp(last_four, "DASH") == 0) {
      return true;
    }
  }
  return false;
}

bool is_base_case(tree_node *node) {
  tree_node *rchild = node->rightmost_child;
  if (rchild == NULL) {
    return true;
  }

  if (rchild->sym.is_terminal == true) {
    return true;
  }
  if (node->parent != NULL && node->sym.nt == node->parent->sym.nt &&
      node->sym.nt != rchild->sym.nt) {
    return true;
  }
  return false;
}

bool is_recursion(tree_node *node) {

  if (node->leftmost_child == NULL) {
    return false;
  }

  tree_node *rchild = node->rightmost_child;
  if (rchild->sym.is_terminal == false) {
    if (rchild->sym.nt == node->sym.nt) {
      return true;
    }
  }

  if (node->parent != NULL && node->sym.nt == node->parent->sym.nt) {
    return true;
  }

  char label[MAX_SYMBOL_LENGTH];
  strcpy(label, non_terminal_string[rchild->sym.nt]);
  if (ends_in_dash(label) == true) {
    // printf("%s 1 dash\n", label);
    return true;
  }

  strcpy(label, non_terminal_string[node->sym.nt]);
  // printf("%s 2 dash\n", label);
  return ends_in_dash(label);
}

bool is_important(tree_node *node) {
  if (node->sym.is_terminal == false) {
    return true;
  }

  if (node->sym.t == NUM || node->sym.t == RNUM || node->sym.t == ID ||
      node->sym.t == GET_VALUE || node->sym.t == PRINT ||
      node->sym.t == INTEGER || node->sym.t == BOOLEAN || node->sym.t == REAL ||
      node->sym.t == EPSILON) {
    return true;
  }
  return false;
}

tree_node *copy_node(tree_node *src) {
  tree_node *dst = create_tree_node();
  dst->token = src->token;
  dst->sym = src->sym;
  return dst;
}

tree_node *extend_inh_node(tree_node *node1, tree_node *node2) {
  if (node1 == NULL) {
    printf("ERROR: In extend_inh_node, node1 = NULL");
    fflush(stdout);
    return NULL;
  }

  if (node2 == NULL) {
    return node1;
  }

  if (node1->node_inh == NULL) {
    node1->node_inh = copy_node(node1->parent);
    node1->node_inh = node2;
    return node1;
  }

  tree_node *temp = node1->node_inh;
  while (temp) {
    if (temp->next == NULL) {
      temp->next = node2;
      return node1;
    }
    temp = temp->next;
  }
  printf("ERROR: while loop in extend_inh_node terminated without exiting.");
  fflush(stdout);
  return node1;
}

tree_node *construct_ast(tree_node *parse_tree_root) {
  tree_node *temp = parse_tree_root;

  do {
    if (temp->visited == false) {

      temp->visited = true;

      tree_node *child1 = NULL;
      tree_node *child2 = temp->leftmost_child;
      while (child2 != NULL) {
        if (is_important(child2) == true) {
          child1 = child2;
          child2 = child2->sibling;
        } else {
          child2 = delete_child(temp, child1, child2);
        }
      }

      if (is_recursion(temp) == true && temp->parent->sym.nt == temp->sym.nt) {
        temp = extend_inh_node(temp, temp->parent->node_inh);
        tree_node *sibling = temp->parent->leftmost_child;

        while (sibling != NULL && sibling->sibling != NULL) {
          if (sibling->sym.is_terminal == false) {
            temp = extend_inh_node(temp, sibling->node_syn);
          } else {
            temp = extend_inh_node(temp, copy_node(sibling));
          }
          sibling = sibling->next;
        }
      }

      if (temp->leftmost_child != NULL) {
        temp = temp->leftmost_child;
      }
    }

    else {
      if (is_recursion(temp) == false) {
        if (temp->leftmost_child == NULL) {
          printf("null: ");
          print_symbol(temp);
          temp->node_syn = create_tree_node();
          temp->node_syn->sym.t = EPSILON;
          temp->node_syn->sym.is_terminal = true;
        } else if (temp->leftmost_child == temp->rightmost_child) {
          if (temp->sym.is_terminal == false) {
            temp->node_syn = temp->leftmost_child->node_syn;
          } else {
            temp->node_syn = copy_node(temp->leftmost_child);
          }
        } else {
          temp->node_syn = copy_node(temp);
          tree_node *child = temp->leftmost_child;
          while (child != NULL) {
            if (temp->sym.is_terminal == false) {
              add_child(temp->node_syn, child->node_syn);
            } else {
              add_child(temp->node_syn, copy_node(child));
            }
            child = child->sibling;
          }
        }
      }

      else {
        tree_node *rchild = temp->rightmost_child;
        if (is_base_case(temp) == false) {
          temp->node_syn = temp->rightmost_child->node_syn;

        } else {
          tree_node *sibling = temp->leftmost_child;

          while (sibling != NULL) {
            if (sibling->sym.is_terminal == false) {
              temp = extend_inh_node(temp, sibling->node_syn);
            } else {
              temp = extend_inh_node(temp, copy_node(sibling));
            }
            sibling = sibling->next;
          }
          if (temp->node_inh == NULL) {
            temp->node_inh = create_tree_node();
            temp->node_inh->sym.is_terminal = true;
            temp->node_inh->sym.t = EPSILON;
          }
          temp->node_syn = temp->node_inh;
        }
      }

      tree_node *child = temp->leftmost_child;
      while (child != NULL) {
        child = delete_child(temp, NULL, child);
      }

      if (temp->sibling != NULL) {
        temp = temp->sibling;
      } else {
        temp = temp->parent;
      }
    }
  } while (temp != NULL);
  return parse_tree_root->node_syn;
}

void print_ast_node(tree_node *node) {
  tree_node *temp = node;
  while (temp != NULL) {
    print_symbol(temp);
    temp = temp->next;
  }
}

void print_ast(tree_node *root) {
  print_ast_node(root);
  tree_node *temp = root->leftmost_child;
  while (temp != NULL) {
    print_ast(temp);
    temp = temp->sibling;
  }
}
