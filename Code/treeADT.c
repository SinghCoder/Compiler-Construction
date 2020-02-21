struct TREENODE{
    struct TREENODE* next= NULL;
    struct TREENODE* child= NULL;
    bool isterminal = false;
    symbol s;
};

typedef struct TREENODE treenode

treenode* rootpointer;

void init_tree() {
    rootpointer = (treenode*)malloc(sizeof(treenode));
    rootpointer->next = NULL;
    rootpointer->child = NULL;
    rootpointer->isterminal = false;
    rootpointer->

}

treenode* add_child(treenode* add_loc)
{
    treenode* temp -add_loc
}