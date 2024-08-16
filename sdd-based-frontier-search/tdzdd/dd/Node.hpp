/*
 * TdZdd: a Top-down/Breadth-first Decision Diagram Manipulation Framework
 * by Hiroaki Iwashita <iwashita@erato.ist.hokudai.ac.jp>
 * Copyright (c) 2014 ERATO MINATO Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <cassert>
#include <stdint.h>
#include <ostream>

namespace tdzdd {
/*
这些常量和掩码的主要目的是使得可以方便地分割和操作一个64位的节点标识，
将其划分为不同的部分，例如行、列和属性。这对于某些数据结构或算法中的位级操作非常有用
*/

int const NODE_ROW_BITS = 20;
int const NODE_ATTR_BITS = 1;
int const NODE_COL_BITS = 64 - NODE_ROW_BITS - NODE_ATTR_BITS;// 43

int const NODE_ROW_OFFSET = NODE_COL_BITS + NODE_ATTR_BITS;// 44
int const NODE_ATTR_OFFSET = NODE_COL_BITS;// 43

// 得到了一个具有20个1的二进制数
uint64_t const NODE_ROW_MAX = (uint64_t(1) << NODE_ROW_BITS) - 1;

// 得到了一个具有43个1的二进制数
uint64_t const NODE_COL_MAX = (uint64_t(1) << NODE_COL_BITS) - 1;

/*
这个常量表示一个掩码，它可以用于提取节点标识中的行部分。
它将最大行值左移行部分的位偏移量，生成一个掩码，用于提取行部分。
*/
uint64_t const NODE_ROW_MASK = NODE_ROW_MAX << NODE_ROW_OFFSET;// 将20个1左移44位

/*
这个常量表示一个掩码，它可以用于提取节点标识中的属性位。
它将1左移属性部分的位偏移量，生成一个掩码，用于提取属性位。
*/
uint64_t const NODE_ATTR_MASK = uint64_t(1) << NODE_ATTR_OFFSET;// 将1向左移43位

class NodeId {
    uint64_t code_;

public:
    NodeId() { // 'code_' is not initialized in the default constructor for SPEED. @suppress("Class members should be properly initialized")
    }

    NodeId(uint64_t code) :
            code_(code) {
    }

    /*
    InitializedNode() :
            Node<ARITY>(NodeId(0, NODE_COL_MAX), NodeId(0, NODE_COL_MAX)) {
    }
    */
    NodeId(uint64_t row, uint64_t col) :
            code_((row << NODE_ROW_OFFSET) | col) {
        assert(row <= NODE_ROW_MAX);
        assert(col <= NODE_COL_MAX);
    }

    NodeId(uint64_t row, uint64_t col, bool attr) :
            code_((row << NODE_ROW_OFFSET) | col) {
        assert(row <= NODE_ROW_MAX);
        assert(col <= NODE_COL_MAX);
        setAttr(attr);
    }

    int row() const {
        return code_ >> NODE_ROW_OFFSET;
    }

    size_t col() const {
        return code_ & NODE_COL_MAX;// NODE_COL_MAX是43个1
    }
    /*
    setAttr 函数根据传入的 val 参数来设置节点标识的属性位。
    如果 val 是 true，则属性位被设置为1，如果 val 是 false，则属性位被设置为0
    */
    void setAttr(bool val) {
        if (val) {
            code_ |= NODE_ATTR_MASK;
        }
        else {
            code_ &= ~NODE_ATTR_MASK;
        }
    }
    /*
    getAttr 函数返回 true，表示属性位被设置为1，返回 false，表示属性位被设置为0。
    这个函数允许查询节点标识对象的属性位的状态
    */
    bool getAttr() const {
        return (code_ & NODE_ATTR_MASK) != 0;
    }

    /*
    这个函数的目的是创建一个新的 NodeId 对象，
    用于表示与原始对象相同的节点，但排除了属性位的影响。
    */
    NodeId withoutAttr() const {
        return code_ & ~NODE_ATTR_MASK;
    }

    bool hasEmpty() const {
        return code_ == 1 || getAttr();
    }

    /*
    这个函数的主要目的是提供一种方法来获取节点标识的编码，但排除了属性位的影响。
    它允许你在不考虑属性位的情况下访问节点标识的编码
    */
    uint64_t code() const {
        return code_ & ~NODE_ATTR_MASK;
    }

    /*
    这个 hash 成员函数用于计算节点标识的哈希值。
    */
    size_t hash() const {
        /*
        code() * 314159257：接下来，它将节点标识的编码乘以一个大素数 314159257。
        这是一个常见的哈希函数计算方法，通过乘以素数可以帮助减少哈希冲突的概率
        */
        return code() * 314159257;
    }

    bool operator==(NodeId const& o) const {
        return code() == o.code();
    }

    bool operator!=(NodeId const& o) const {
        return !(*this == o);
    }

    bool operator<(NodeId const& o) const {
        return code() < o.code();
    }

    bool operator>=(NodeId const& o) const {
        return !(*this < o);
    }

    bool operator>(NodeId const& o) const {
        return o < *this;
    }

    bool operator<=(NodeId const& o) const {
        return !(o < *this);
    }

    /*
    打印 NodeId 对象的信息
    */
    friend std::ostream& operator<<(std::ostream& os, NodeId const& o) {
        os << o.row() << ":" << o.col();
        /*
        用于检查 NodeId 对象的属性位是否被设置为1。如果属性位被设置为1，
        就在流中输出一个加号 +，表示该节点具有某种属性。
        */
        if (o.code_ & NODE_ATTR_MASK) os << "+";
        return os;
    }
};

struct NodeBranchId {
    size_t col;
    int row;
    int val;

    NodeBranchId() { // @suppress("Class members should be properly initialized")
    }

    NodeBranchId(int row, size_t col, int val) :
            col(col), row(row), val(val) {
    }
};

template<int ARITY>
struct Node {
    NodeId branch[ARITY];

    Node() {
    }

    Node(NodeId f0, NodeId f1) {
        branch[0] = f0;
        for (int i = 1; i < ARITY; ++i) {
            branch[i] = f1;
        }
    }

    Node(NodeId const* f) {
        for (int i = 0; i < ARITY; ++i) {
            branch[i] = f[i];
        }
    }

    size_t hash() const {
        size_t h = branch[0].code();
        for (int i = 1; i < ARITY; ++i) {
            h = h * 314159257 + branch[i].code() * 271828171;
        }
        return h;
    }

    bool operator==(Node const& o) const {
        for (int i = 0; i < ARITY; ++i) {
            if (branch[i] != o.branch[i]) return false;
        }
        return true;
    }

    bool operator!=(Node const& o) const {
        return !operator==(o);
    }

    friend std::ostream& operator<<(std::ostream& os, Node const& o) {
        os << "(" << o.branch[0];
        for (int i = 1; i < ARITY; ++i) {
            os << "," << o.branch[i];
            //os << "," << o.branch[i].row() << ":" << o.branch[i].col();
        }
        return os << ")";
    }
};

template<int ARITY>
struct InitializedNode: Node<ARITY> {
    InitializedNode() :
            Node<ARITY>(NodeId(0, NODE_COL_MAX), NodeId(0, NODE_COL_MAX)) {
    }

    InitializedNode(NodeId f0, NodeId f1) :
            Node<ARITY>(f0, f1) {
    }

    InitializedNode(Node<ARITY> const& o) :
            Node<ARITY>(o.branch) {
    }
};

} // namespace tdzdd
