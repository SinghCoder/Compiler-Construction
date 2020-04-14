#include "ast.h"
#include "astDef.h"
#include "parser.h"
#include "treeADT.h"
#include <stdlib.h>
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
  if (node == NULL)
	return false;
  if (node->sym.is_terminal == true ||
	  node->parent == NULL) { // A -> a me a and S->.. me S
	return false;
  }

  if ((rchild != NULL && rchild->sym.is_terminal == false) &&
	  node->sym.nt == rchild->sym.nt) // A -> BA1 me A
	return false;

  if (ends_in_dash(non_terminal_string[node->sym.nt])) { // A -> B A' me A
	return true;
  }

  if (node->sym.nt ==
	  node->parent->sym.nt) { // A -> B A1 me A1, if it reaches here it means A1
							  // is base case for A
	return true;
  }

  return false; // A-> BC me B and C
}

bool is_recursion(tree_node *node) {
  if (node == NULL)
	return false;

  if (is_base_case(node) == true) {
	return true;
  }

  if (node->sym.is_terminal == true) {
	return false;
  }

  tree_node *rchild = node->rightmost_child;
  if (rchild != NULL) {
	if ((rchild->sym.is_terminal == false) &&
		(rchild->sym.nt == node->sym.nt ||
		 ends_in_dash(non_terminal_string[rchild->sym.nt]))) {
	  return true;
	}
  }
  return false;
}

bool is_useful_terminal(token_name t) {
  switch (t) {
  case NUM:
  case RNUM:
  case ID:
  case GET_VALUE:
  case PRINT:
  case INTEGER:
  case BOOLEAN:
  case REAL:
  case EPSILON:
  case TRUE:
  case FALSE:
  case AND:
  case OR:
  case LT:
  case LE:
  case GE:
  case GT:
  case EQ:
  case NE:
  case PLUS:
  case MINUS:
  case MUL:
  case DIV:
  case DRIVER:
  case DEFAULT:

	return true;
	break;
  default:
	return false;
	break;
  }
}

bool is_important(tree_node *node) {
  if (node->sym.is_terminal == false)
	return true;

  return is_useful_terminal(node->sym.t);
}

tree_node *copy_node(tree_node *src) {
  tree_node *dst = create_tree_node();
  dst->token = src->token;
  dst->sym = src->sym;
  dst->line_nums.start = src->line_nums.start;
  dst->line_nums.end = src->line_nums.end;
  return dst;
}

void extend_inh_node(tree_node *node1, tree_node *node2) {

  if (node2 == NULL) { // recursion ki root pe node_inh kuch mat banao
	return;
  }

  if (node1->node_inh ==
	  NULL) { // list doesn't exist already, create a node with label
	node1->node_inh = copy_node(node1->parent);
  }
  add_child(node1->node_inh, node2); // append element to list
  return;
}

tree_node *construct_ast(tree_node *parse_tree_root) {
  tree_node *temp = parse_tree_root;

  do {
	// print_symbol(temp->sym);printf("\n");
	// if(temp->sym.is_terminal && temp->sym.t == ID)
	//   printf(", %s, s : %d, e : %d\n", temp->token.id.str, temp->line_nums.start, temp->line_nums.end);
	// printf(", curr_start : %d, curr_end : %d\n",curr_start, curr_end);
	tree_node *lchild = temp->leftmost_child;
	tree_node *rchild = temp->rightmost_child;
	/**
	 * @brief Visiting the node first time
	 * 
	 */    
	if (temp->visited == false) {


	  if(temp->sym.is_terminal && temp->sym.t == ID){   // assign begin and end line nums to this id if it's part of declarestmt
		  tree_node *tmp_node = temp;          
		  while(tmp_node){
			if(tmp_node->sym.nt == DECLARESTMT){
			  temp->line_nums.start = curr_start;
			  temp->line_nums.end = curr_end;
			  break;
			}
			tmp_node = tmp_node->parent;
		  }
	  }

	  temp->visited = true;

	  tree_node *prev_child = NULL;
	  tree_node *curr_child = lchild;
	  /**
	   * @brief Remove unnecessary children of current node
	   * 
	   */
	  while (curr_child != NULL) {

		if (is_important(curr_child) == true) {
		  prev_child = curr_child;
		  curr_child = curr_child->sibling;
		} else {
		  if(curr_child->sym.is_terminal){
			if(curr_child->sym.t == START){
			  curr_start = curr_child->token.line_no;
			}
			else if(curr_child->sym.t == END){
			  curr_end = curr_child->token.line_no;
			}
		  } 
		  curr_child = delete_child(temp, prev_child, curr_child);
		}

	  } // end of while curr_child != NULL

	  if(temp->sym.nt == MODULEDEF){
		  // printf("assigned to module name %s, %d, %d\n", temp->parent->leftmost_child->token.id.str, curr_start, curr_end );
		  temp->parent->leftmost_child->line_nums.start = curr_start;
		  temp->parent->leftmost_child->line_nums.end = curr_end;
	  }
	  
	  lchild = temp->leftmost_child;
	  rchild = temp->rightmost_child;
	  
	  /**
	   * @brief Node is derived from recursive rule
	   * assign node_inh values by taking from parent and appeding all it's left neighbors
	   * i.e. creating linked list
	   */

	  if ((is_recursion(temp) == true) &&
		  (temp->parent->sym.nt == temp->sym.nt ||
		   ends_in_dash(non_terminal_string[temp->sym.nt]) == true)) {

		temp->node_inh = temp->parent->node_inh;

		tree_node *sibling = temp->parent->leftmost_child;
		/**
		 * @brief Append left neighbors' node_syns
		 * 
		 */
		while (sibling != temp) {
		  if (sibling->sym.is_terminal == false) {
			extend_inh_node(temp, sibling->node_syn);
		  } else {
			extend_inh_node(temp, copy_node(sibling));
		  }
		  sibling = sibling->sibling;
		} // end of while sibling != temp

		if (is_base_case(temp) == true) {
		  tree_node *child = lchild;

		  // }
		  while (child != NULL) {            
			if (child->sym.is_terminal == false) {
			  extend_inh_node(temp, child->node_syn);
			} else {
			  if (child->sym.t != EPSILON || (temp->node_inh == NULL && !ends_in_dash(non_terminal_string[temp->sym.nt]))) {
				extend_inh_node(temp, copy_node(child));
			  }
			}
			child = child->sibling;
		  } // end of while child != NULL
		}   // if is base case temp
	  } // if is recursion and temp's parent's nt = temp's nt || ends in dash

	  if (lchild != NULL) {
		temp = lchild;
	  }
	} // end of if temp->visited = false
	/**
	 * @brief Visiting node again
	 * i.e. all children have been visited
	 */
	else {      
	  temp->visited = false;
	  if (is_recursion(temp) == false) {
		/**
		 * @brief Chain of non-terminals
		 * 
		 */
		if (lchild != NULL && (lchild == rchild)) {

		  if (lchild->sym.is_terminal ==
			  false) { // It's a chain of nts, simple pass synthesized nodes
					   // A->b => A.node_syn = B.node_syn
			temp->node_syn = lchild->node_syn;
		  }

		  else {
			temp->node_syn = copy_node(
				lchild); // MD -> id type rule, initialize syn_attr of MD
		  }
		} else { // visit karke a chuka and not recursion

		  temp->node_syn = copy_node(temp); // label create kardo
		  tree_node *child = lchild;

		  while (child != NULL) { // sare imp walo ke node-syn ko append kardo            
			// printf("\n");
			if (child->sym.is_terminal == false) {
			  add_child(temp->node_syn, child->node_syn);
			}

			else {
			  add_child(temp->node_syn, copy_node(child));
			}
			child = child->sibling;
		  }
		}
	  } // if is recursion temp = false

	  else {
		if (is_base_case(temp) ==
			false) { // returning from recursion , do A->BA1, A1.node_syn =
					 // A1.node_inh and A.node_syn = A1.node_syn
		  temp->node_syn = rchild->node_syn;

		} else {
		  temp->node_syn = temp->node_inh;
		}
	  } // is recursion temp = true

	  tree_node *child = lchild;
	  while (child != NULL) {

		child = delete_child(
			temp, NULL,
			child); // free all children, no parse tree node is preserved to
					// avoid nullifying parents/siblings fields
	  }
	tree_node *ast_node = temp->node_syn;
	  if( (ast_node != NULL) && 
	  		(ast_node->leftmost_child == ast_node->rightmost_child) &&
			  (ast_node->leftmost_child != NULL) &&
			  	(ast_node->leftmost_child->sym.is_terminal == false) ){// It's a chain of non-terminals in AST, remove it

			tree_node *ast_child = ast_node->leftmost_child;
		ast_child->parent = ast_node->parent;
		ast_child->sibling = ast_node->sibling;
		temp->node_syn = ast_child;
		free(ast_node);

		}

	  if (temp->sibling != NULL) {
		temp = temp->sibling;
	  } else {
		temp = temp->parent;
	  }
	} // end of else for temp->visited = true

  } while (temp != NULL);
  return parse_tree_root->node_syn;
}

void print_ast(tree_node *root) {
  print_symbol(root->sym);
  tree_node *temp = root->leftmost_child;
  while (temp != NULL) {
	print_ast(temp);
	temp = temp->sibling;
  }
  // printf("over: ");
  // print_symbol(root->sym);
}

void traverse_ast(tree_node *ast_node)
{
    while(ast_node != NULL){
        if(ast_node->visited == false){
			num_tree_nodes++;
            ast_node->visited = true;
            if(ast_node->leftmost_child != NULL){
                ast_node = ast_node->leftmost_child;
            }
        }
        else{
            ast_node->visited = false;
            if(ast_node->sibling){
                ast_node = ast_node->sibling;
            }
            else{
                ast_node = ast_node->parent;
            }
        }

    }
}	