#ifndef DISJOINT_SET_H
#define DISJOINT_SET_H

class DSSetList {

public:
  struct Node {
    Node *next;
    Node *prev;
    int id;
  };

  DSSetList (int capacity) {
    
    size = 0;
    index = new Node*[capacity];
    head = 0;
    
    for (int i = 0; i < capacity; i++) {
      Node *n = new Node;
      n->id = i;
      n->next = n->prev = 0;
      index[i] = n;
    }
  }

  ~DSSetList () {
    
    while (head) {
      Node *del = head;
      head = head->next;
      delete del;
    }
    delete [] index;
  }

  void del(int set_id) {

    Node *node = index[set_id];

    if (node == head) {
      head = head->next;
    }
    if (node->prev) {
      node->prev->next = node->next;
    }
    if (node->next) {
      node->next->prev = node->prev;
    }
    node->next = node->prev = 0;
    size--;
  }
  
  void add(int set_id) {
    
    Node *node = index[set_id];

    if (head) {
      head->prev = node;
    }
    node->next = head;
    head = node;
    size++;
  }

  int size;
  Node *head;
  Node **index;
};

class DSSetIterator {
  
public:
  
  bool hasNext() {
    return ptr;
  }
  
  int next() {
  
    int id = ptr->id;
    ptr = ptr->next;
    return id;
  }
  DSSetList::Node *ptr;
};

class DSMemberList {

public:
  struct Node {
    Node *next;
    int id;
  };

  DSMemberList() {
    
    size = 0;
    head = tail = 0;
  }
  
  ~DSMemberList() {
    
    while (head) {
      Node *del = head;
      head = head->next;
      delete del;
    }
  }
  
  void add(int x) {
    
    Node *n = new Node;
    n->id = x;
    n->next = 0;
    
    if (!head) {
      head = tail = n;
    } else {
      tail->next = n;
      tail = n;
    }
    size++;
  }

  void append(DSMemberList *list) {
  
    tail->next = list->head;
    tail = list->tail;
    list->head = list->tail = 0;
    size += list->size;
  }
  
  int size;
  Node *head;
  Node *tail;
};

class DSMemberIterator {

public:
  
  bool hasNext() {
    return ptr;
  }
  
  int next() {
  
    int id = ptr->id;
    ptr = ptr->next;
    return id;
  }
  DSMemberList::Node *ptr;
};

class DisjointSet {

public:
  DisjointSet(int n) {

    this->num_sets = n;
    set = new int[n];
    rank = new int[n];
    set_ids = new DSSetList (n);
    set_members = new DSMemberList[n];
    
    for (n--; n >= 0; n--) {
      set[n] = n;
      rank[n] = 0;
      set_members[n].add(n);
      set_ids->add(n);
    }
  }

  ~DisjointSet() {

    delete set_ids;
    delete [] set_members;
    delete [] set;
    delete [] rank;
  }

  int merge(int x, int y) {

    x = find(x);
    y = find(y);
    
    if (x == y) {
      return -1;
    }
    if (rank[x] <= rank[y]) {
      rank[y] += rank[x] == rank[y];
      x ^= y; y ^= x; x ^= y;
    }
    --num_sets;
    set[y] = x;
    set_ids->del(y);
    set_members[x].append(&set_members[y]);
    return x;
  }

  int find(int x) {
  
    int i = 0, path[8];

    while (x != set[x]) {
      path[i++] = x;
      x = set[x];
    }
    while (i) {
      set[path[--i]] = x;
    }
    return x;
  }

  int size(int x) {
    return set_members[find(x)].size;
  }

  DSSetIterator sets() {
  
    DSSetIterator it;
    it.ptr = set_ids->head;
    return it;
  }

  DSMemberIterator members(int x) {
    
    DSMemberIterator it;
    it.ptr = set_members[x].head;
    return it;
  }
  
  DSMemberList *set_members;
  DSSetList  *set_ids;
  int num_sets;
  int *set;
  int *rank;
};

#endif
