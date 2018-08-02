#ifndef __HUFFMANTREE_H__
#define __HUFFMANTREE_H__

#include <iostream>
#include <stdio.h>
#include <queue>
#include <vector>

template<class W>
class HuffmanTreeNode{
public:
	HuffmanTreeNode<W>* _left;
	HuffmanTreeNode<W>* _right;
	HuffmanTreeNode<W>* _parent;
	W _w;

	HuffmanTreeNode(const W& w)
		:_left(NULL)
		, _right(NULL)
		, _parent(NULL)
		, _w(w)
	{}
};

template<class W>
class HuffmanTree{
	typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree()
		:_root(NULL)
	{}

	struct NodeCompare{
		bool operator()(const Node* l, const Node* r){
			return l->_w > r->_w;
		}
	};

	HuffmanTree(W* w, size_t n, W& invalid)
	{
		// ¹¹½¨¹þ¸¥ÂüÊ÷
		priority_queue<Node*, vector<Node*>, NodeCompare> minHead;
		size_t i = 0;
		for (i = 0; i < n; ++i){
			if (w[i] != invalid)
				minHead.push(new Node(w[i]));
		}

		while (minHead.size() > 1){
			Node* left = minHead.top();
			minHead.pop();
			Node* right = minHead.top();
			minHead.pop();
			Node* parent = new Node(left->_w + right->_w);
			parent->_left = left;
			parent->_right = right;
			minHead.push(parent);
		}
		//_root = minHead.top();
	}

	~HuffmanTree()
	{
		Destory(_root);
		_root = NULL;
	}

	void Destory(Node* root)
	{
		if (root == NULL){
			return;
		}
		Destory(root->_left);
		Destory(root->_right);
		delete root;
	}

	Node* GetRoot()
	{
		return _root;
	}
private:
	HuffmanTree(const HuffmanTree<W>& t);
	HuffmanTree& operator=(const HuffmanTree<W>& t);
protected:
	Node* _root;
};

#endif //__HUFFMANTREE_H__
