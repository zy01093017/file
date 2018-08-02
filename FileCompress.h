#ifndef __FILECOMPRESS_H__
#define __FILECOMPRESS_H__

#include "HuffmanTree.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <assert.h>
#include <string>
using namespace std;

typedef long long LongType;

struct CharInfo{
	char _ch;        //字符
	string _code;    //编码
	LongType _count; //出现次数

	// 重载+ 是为了在构建哈夫曼树时，parent = left+right
	CharInfo operator+(const CharInfo& info)
	{
		CharInfo ret;
		ret._count = _count + info._count;
		return ret;
	}

	bool operator>(const CharInfo& info) const
	{
		return _count > info._count;
	}

	bool operator != (const CharInfo& invalid)
	{
		return _count != invalid._count;
	}
};

class FileCompress{
	typedef HuffmanTreeNode<CharInfo> Node;

public:
	struct configInfo
	{
		char _ch;
		LongType _count;
	};
	FileCompress()
	{
		size_t i = 0;
		for (i = 0; i < 256; ++i){
			_hashInfos[i]._ch = i;
			_hashInfos[i]._count = 0;
		}
	}
	void Compress(const char* file)
	{
		// 1.统计文件中字符出现的次数
		ifstream ifs(file, ios_base::in | ios_base::binary);
		char ch;
		while (ifs.get(ch)){
			_hashInfos[(unsigned char)ch]._count++;
		}

		// 2.生成哈弗曼树
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_hashInfos, 256, invalid);

		// 3.生成 Huffman code
		generateHuffmanCode(tree.GetRoot());

		// 4.压缩
		string compressFile = file;
		compressFile += ".huffman";
		ofstream ofs(compressFile.c_str(), ios_base::out | ios_base::binary);

		// 把字符和次数也写到压缩文件中，方便解压时生成 _hashInfos
		for (size_t i = 0; i < 256; ++i){
			if (_hashInfos[i]._count > 0){
				configInfo info;
				info._ch = _hashInfos[i]._ch;
				info._count = _hashInfos[i]._count;
				ofs.write((char*)&info, sizeof(configInfo));
			}
		}
		configInfo end;
		end._count = 0;
		ofs.write((char*)&end, sizeof(configInfo));

		ifs.clear();  //清理一下，下面seekg才起作用
		ifs.seekg(0);
		char value = 0;
		int pos = 0;

		//把编码写入到压缩文件中
		while (ifs.get(ch)){
			string& code = _hashInfos[(unsigned char)ch]._code;
			for (size_t i = 0; i < code.size(); ++i){
				if (code[i] == '0') value &= (~(1 << pos));
				else if (code[i] == '1') value |= (1 << pos);
				else assert(false);
				
				++pos;
				if (pos == 8){
					ofs.put(value);
					pos = 0;
					value = 0;
				}
			}
		}
		// 如果最后一个字节没填满，直接放进去
		if (pos > 0) ofs.put(value);
	}


	void generateHuffmanCode(Node* root)
	{
	    if(root == nullptr)
	        return;
	    
	    if(!root->_left && !root->_right){
			_hashInfos[(unsigned char)root->_w._ch]._code = root->_w._code;
	        return;
	    }

	    // 往左边走，把左孩子的 code 加上'0'
	    if(root->_left){
	        root->_left->_w._code = root->_w._code + '0';
	        generateHuffmanCode(root->_left);
	    }
	
	    // 往右边走，把右孩子的 code 加上'1'
	    if(root->_right){
	        root->_right->_w._code = root->_w._code + '1';
	        generateHuffmanCode(root->_right);
	    }
	}

	void Uncompress(const char* file)
	{
		// 1. 打开压缩文件，进行解压
		ifstream ifs(file, ios_base::in | ios_base::binary);
		string uncompressfile = file;
		size_t pos = uncompressfile.rfind('.');
		assert(pos != string::npos);
		uncompressfile.erase(pos);

		uncompressfile += ".unhuffman";
		ofstream ofs(uncompressfile.c_str(), ios_base::out | ios_base::binary);
		
		// 重构 _hashInfos
		while (1){
			configInfo info;
			ifs.read((char*)&info, sizeof(configInfo));
			if (info._count > 0)
			{
				_hashInfos[(unsigned char)info._ch]._count = info._count;
			}
			else
			{
				break;
			}
		}

		// 2. 重建Huffman Tree
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_hashInfos, 256, invalid);

		// 3. 根据 Huffman Code 解压缩
		char ch;
		Node* root = tree.GetRoot();
		LongType fileCount = root->_w._count;  //记录了文件总字符的个数，为解压做准备

		Node* cur = root;
		while (ifs.get(ch)){
			for (size_t pos = 0; pos < 8; ++pos){

				if (ch & (1 << pos)) cur = cur->_right;  // 1
				else cur = cur->_left;                   // 0

				if (!cur->_left && !cur->_right) {       //
					ofs.put(cur->_w._ch);
					cur = root;

					if (--fileCount == 0) break; // 解压完成
				}
			}
		}
	}
private:
	CharInfo _hashInfos[256];
};

void TestFileCompress()
{
	FileCompress fc;
	fc.Compress("Input.txt");
}

void TestFileUncompress()
{
	FileCompress fc;
	fc.Uncompress("Input.txt.huffman");
}

#endif //__FILECOMPRESS_H__
