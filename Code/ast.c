#include "ast.h"
#include "astDef.h"
#include "treeADT.h"
#include <string.h>

bool ends_in_dash(char *label) {
  int len = strlen(label);
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

  if (node->sym.is_terminal == true || node->parent == NULL) {
    return false;
  }

  if (node->sym.nt == rchild->sym.nt)
    return false;

  if (ends_in_dash(non_terminal_string[node->sym.nt])) {
    return true;
  }

  if (node->sym.nt == node->parent->sym.nt) {
    return true;
  }

  return false;
}

bool is_recursion(tree_node *node) {

  if (is_base_case(node) == true) {
    return true;
  }

  if (node->sym.is_terminal == true) {
    return false;
  }

  tree_node *rchild = node->rightmost_child;
  if (rchild != NULL && rchild->sym.is_terminal == false) {
    if (rchild->sym.nt == node->sym.nt) {
      return true;
    }
  }

  if (ends_in_dash(non_terminal_string[rchild->sym.nt]) == true) {
    return true;
  }

  return false;
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

void extend_inh_node(tree_node *node1, tree_node *node2) {

  if (node2 == NULL) {
    return;
  }

  if (node1->node_inh == NULL) {
    node1->node_inh = copy_node(node1->parent);
  }

  add_child(node1->node_inh, node2);
  return;
}

tree_node *construct_ast(tree_node *parse_tree_root) {
  tree_node *temp = parse_tree_root;

  do {
    // printf("temp is: ");
    // print_symbol(temp);

    tree_node *lchild = temp->leftmost_child;
    tree_node *rchild = temp->rightmost_child;

    if (temp->visited == false) {

      temp->visited = true;

      tree_node *child1 = NULL;
      tree_node *child2 = lchild;
      while (child2 != NULL) {
        if (is_important(child2) == true) {
          child1 = child2;
          child2 = child2->sibling;
        } else {
          child2 = delete_child(temp, child1, child2);
        }
      }

      lchild = temp->leftmost_child;
      rchild = temp->rightmost_child;

      if (is_recursion(temp) == true &&
          !(temp->parent->sym.nt != temp->sym.nt &&
            ends_in_dash(non_terminal_string[temp->sym.nt]) == false)) {
        temp->node_inh = temp->parent->node_inh;
        tree_node *sibling = temp->parent->leftmost_child;

        while (sibling != temp) {
          if (sibling->sym.is_terminal == false) {
            extend_inh_node(temp, sibling->node_syn);
          } else {
            extend_inh_node(temp, copy_node(sibling));
          }
          sibling = sibling->sibling;
        }

        if (is_base_case(temp) == true) {
          tree_node *child = lchild;

          while (child != NULL) {
            if (child->sym.is_terminal == false) {
              extend_inh_node(temp, child->node_syn);
            } else {
              extend_inh_node(temp, copy_node(child));
            }
            child = child->sibling;
          }
        }
      }

      if (lchild != NULL) {
        temp = lchild;
      }
    }

    else {
      if (is_recursion(temp) == false) {
        if (lchild == NULL) {
          // printf("null: ");
          // print_symbol(temp);
          temp->node_syn = create_tree_node();
          temp->node_syn->sym.t = EPSILON;
          temp->node_syn->sym.is_terminal = true;
        } else if (lchild == rchild) {
          if (lchild->sym.is_terminal == false) {
            temp->node_syn = lchild->node_syn;
          } else {
            temp->node_syn = copy_node(lchild);
          }
        } else {
          temp->node_syn = copy_node(temp);
          tree_node *child = lchild;
          while (child != NULL) {
            if (child->sym.is_terminal == false) {
              add_child(temp->node_syn, child->node_syn);
            } else {
              add_child(temp->node_syn, copy_node(child));
            }
            child = child->sibling;
          }
        }
      }

      else {
        if (is_base_case(temp) == false) {
          temp->node_syn = rchild->node_syn;

        } else {
          temp->node_syn = temp->node_inh;
        }
      }

      tree_node *child = lchild;
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

void print_ast(tree_node *root) {
  print_symbol(root);
  tree_node *temp = root->leftmost_child;
  while (temp != NULL) {
    print_ast(temp);
    temp = temp->sibling;
  }
  // printf("over: ");
  // print_symbol(root);
}
