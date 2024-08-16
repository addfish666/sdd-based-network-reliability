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

#include <algorithm>
#include <cassert>
#include <climits>
#include <ostream>
#include <stdexcept>

#include "Node.hpp"
#include "DataTable.hpp"
#include "../util/MyVector.hpp"

namespace tdzdd {

template<int ARITY>
class NodeTableEntity: public DataTable<Node<ARITY> > {
    /*
    在C++中，mutable 是一个关键字，用于修饰类的成员变量。
    当一个成员变量被声明为 mutable 时，它表明这个变量可以在类的 const 成员函数中被修改
    */
    mutable MyVector<MyVector<int> > higherLevelTable;
    mutable MyVector<MyVector<int> > lowerLevelTable;

public:
    /**
     * Constructor.
     * @param n the number of rows.
     */
    /*
    NodeTableEntity(int n = 1)：这是 NodeTableEntity 类的构造函数。它接受一个整数参数 n，
    表示行数，默认值为1。在构造函数中，它首先调用基类 DataTable<Node<ARITY>> 的构造函数，
    并传递了 n 作为参数。然后，它使用 assert 函数来检查 n 是否大于等于1，
    以确保行数是有效的。最后，它调用 initTerminals() 函数来初始化一些终端节点。
    */
    NodeTableEntity(int n = 1)
            : DataTable<Node<ARITY> >(n) {
        assert(n >= 1);
        initTerminals();
    }

    /**
     * Clears and initializes the table.
     * @param n the number of rows.
     */
    void init(int n) {
        assert(n >= 1);
        DataTable<Node<ARITY> >::init(n);
        initTerminals();
    }

    /**
     * Initializes the terminal nodes.
     */
    void initTerminals() {
        /*
        这行代码创建了一个引用 t，
        它指向了 NodeTableEntity 类的实例中的第一个元素。 (*this) 代表当前对象
        */
        MyVector<Node<ARITY> >& t = (*this)[0];
        t.resize(2);
        for (int j = 0; j < 2; ++j) {
            t[j] = Node<ARITY>(j, j);
        }
    }

//    /**
//     * Gets the variable ID at a given level.
//     * @param level level.
//     * @return variable ID.
//     */
//    /*
//    对于一个BDD，变量x是从下标0开始到n结束，下标0对于最高的level为n+1
//    */
//    int varAtLevel(int level) const {
//        assert(0 <= level && level <= numVars());
//        return (level == 0) ? INT_MAX : numVars() - level;
//    }
// 
//    /**
//     * Gets the level of a variable.
//     * @param var variable ID.
//     * @return level.
//     */
//    int levelOfVar(int var) const {
//        assert((0 <= var && var < numVars()) || var == INT_MAX);
//        return (var == INT_MAX) ? 0 : numVars() - var;
//    }

    /**
     * Gets the number of nonterminal nodes.
     * @return the number of nonterminal nodes.
     */
    size_t size() const {
        return this->totalSize() - (*this)[0].size();
    }

    /**
     * Gets the number of ZDD variables.
     * @return the number of ZDD variables.
     */
    int numVars() const {
        return this->numRows() - 1;
    }

    /**
     * Changes the number of ZDD variables
     * by shifting up/down the levels of existing variables.
     * @param n required number of variables.
     */
    /*
    stretchBottom 函数用于改变 ZDD 的变量数量，通过移动节点和调整行的数量以适应新的变量数量
    */
    void stretchBottom(int n) {
        int n0 = numVars();
        int d = n - n0;

        if (d > 0) {
            this->setNumRows(n + 1);

            for (int i = n0; i > 0; --i) {
                size_t m = (*this)[i].size();
                this->initRow(i + d, m);

                for (size_t j = 0; j < m; ++j) {
                    for (int b = 0; b < ARITY; ++b) {
                        NodeId ff = child(i, j, b);
                        int ii = ff.row();
                        // ii == 0 代表其为终端节点
                        child(i + d, j, b) =
                                (ii == 0) ? ff : NodeId(ii + d, ff.col());
                    }
                }

                this->initRow(i, 0);
            }
        }
        else if (d < 0) {
            for (int i = 1 - d; i <= n0; ++i) {
                size_t m = (*this)[i].size();
                this->initRow(i + d, m);

                for (size_t j = 0; j < m; ++j) {
                    for (int b = 0; b < ARITY; ++b) {
                        NodeId ff = child(i, j, b);
                        int ii = ff.row();
                        child(i + d, j, b) =
                                (ii == 0) ? ff :
                                (ii + d <= 0) ? 1 : NodeId(ii + d, ff.col());
                    }
                }

                this->initRow(i, 0);
            }

            this->setNumRows(n + 1);
        }
    }

    /**
     * Gets a node.
     * @param f node ID.
     * @return node @p f.
     */
    Node<ARITY> const& node(NodeId f) const {
        return (*this)[f.row()][f.col()];
    }

    /**
     * Gets a reference to a node.
     * @param f node ID.
     * @return node @p f.
     */
    Node<ARITY>& node(NodeId f) {
        return (*this)[f.row()][f.col()];
    }

    /**
     * Gets a child node ID.
     * @param f parent node ID.
     * @param b child branch.
     * @return the @p b-child of @p f.
     */
    NodeId child(NodeId f, int b) const {
        return child(f.row(), f.col(), b);
    }

    /**
     * Gets a reference to a child node ID.
     * @param f parent node ID.
     * @param b child branch.
     * @return the @p b-child of @p f.
     */
    NodeId& child(NodeId f, int b) {
        return child(f.row(), f.col(), b);
    }

    /**
     * Gets a child node ID.
     * @param i parent row.
     * @param j parent column.
     * @param b child branch.
     * @return the @p b-child of the parent.
     */
    NodeId child(int i, size_t j, int b) const {
        assert(0 <= b && b < ARITY);
        return (*this)[i][j].branch[b];
    }

    /**
     * Gets a reference to a child node ID.
     * @param i parent row.
     * @param j parent column.
     * @param b child branch.
     * @return the @p b-child of the parent.
     */
    NodeId& child(int i, size_t j, int b) {
        assert(0 <= b && b < ARITY);
        return (*this)[i][j].branch[b];
    }

    /**
     * Gets a descendant node ID by tracing 0-edges.
     * @param f parent node ID.
     * @param stopLevel level to stop going down.
     * @return reached node ID.
     */
    NodeId getZeroDescendant(NodeId f, int stopLevel) const {
        assert(0 <= stopLevel);
        if (stopLevel == 0 && f.hasEmpty()) return 1;
        while (f.row() > stopLevel) {
            f = child(f, 0);
        }
        return f;
    }

    /**
     * Deletes current index information.
     */
    void deleteIndex() {
        higherLevelTable.clear();
        lowerLevelTable.clear();
    }

    /**
     * Makes index information.
     * @param useMP use an algorithm for multiple processors.
     */
    void makeIndex(bool useMP = false) const {
        int const n = this->numRows() - 1;
        higherLevelTable.clear();
        higherLevelTable.resize(n + 1);
        lowerLevelTable.clear();
        lowerLevelTable.resize(n + 1);
        MyVector<bool> lowerMark(n + 1);

        for (int i = n; i >= 1; --i) {
            MyVector<Node<ARITY> > const& node = (*this)[i];
            size_t const m = node.size();
            int lowest = i;
            MyVector<bool> myLower(n + 1);

#ifdef _OPENMP
            if (useMP) {
#pragma omp parallel for schedule(static)
                for (intmax_t j = 0; j < intmax_t(m); ++j) {
                    for (int b = 0; b < ARITY; ++b) {
                        int const ii = node[j].branch[b].row();
                        if (ii == 0) continue;
                        if (ii < lowest) {
#pragma omp critical
                            if (ii < lowest) lowest = ii;
                        }
                        if (!lowerMark[ii]) {
                            myLower[ii] = true;
                            lowerMark[ii] = true;
                        }
                    }
                }
            }
            else
#endif
            for (size_t j = 0; j < m; ++j) {
                for (int b = 0; b < ARITY; ++b) {
                    int const ii = node[j].branch[b].row();
                    if (ii == 0) continue;
                    if (ii < lowest) lowest = ii;
                    if (!lowerMark[ii]) {
                        myLower[ii] = true;
                        lowerMark[ii] = true;
                    }
                }
            }

            higherLevelTable[lowest].push_back(i);
            MyVector<int>& lower = lowerLevelTable[i];
            for (int ii = lowest; ii < i; ++ii) {
                if (myLower[ii]) lower.push_back(ii);
            }
        }
    }

    /**
     * Returns a collection of the higher levels that directly refers
     * the given level and that does not refer any lower levels.
     * @param level the level.
     */
    MyVector<int> const& higherLevels(int level) const {
        if (higherLevelTable.empty()) makeIndex();
        return higherLevelTable[level];
    }

    /**
     * Returns a collection of the lower levels that are referred
     * by the given level and that are not referred directly by
     * any higher levels.
     * @param level the level.
     */
    MyVector<int> const& lowerLevels(int level) const {
        if (lowerLevelTable.empty()) makeIndex();
        return lowerLevelTable[level];
    }

    /**
     * Dumps the node table in Graphviz (dot) format.
     * @param os output stream.
     * @param title title label.
     */
    void dumpDot(std::ostream& os, std::string title = "") const {
        os << "digraph \"" << title << "\" {\n";
        for (int i = this->numRows() - 1; i >= 1; --i) {
            os << "  " << i << " [shape=none];\n";
        }
        for (int i = this->numRows() - 2; i >= 1; --i) {
            os << "  " << (i + 1) << " -> " << i << " [style=invis];\n";
        }

        if (!title.empty()) {
            os << "  labelloc=\"t\";\n";
            os << "  label=\"" << title << "\";\n";
        }

        bool terminal1 = false;

        for (int i = this->numRows() - 1; i > 0; --i) {
            size_t m = (*this)[i].size();

            for (size_t j = 0; j < m; ++j) {
                NodeId f = NodeId(i, j);
                os << "  \"" << f << "\";\n";

                for (int b = 0; b < ARITY; ++b) {
                    NodeId ff = child(i, j, b);
                    bool aa = ff.getAttr();
                    if (ff == 0) continue;

                    if (ff == 1) {
                        terminal1 = true;
                        os << "  \"" << f << "\" -> \"$\"";
                    }
                    else {
                        ff.setAttr(false);
                        os << "  \"" << f << "\" -> \"" << ff << "\"";
                    }

                    os << " [style=";
                    if (b == 0) {
                        os << "dashed";
                    }
                    else {
                        os << "solid";
                        if (ARITY > 2) {
                            os << ",color="
                                    << ((b == 1) ? "blue" :
                                        (b == 2) ? "red" : "green");
                        }
                    }
                    if (aa) os << ",arrowtail=dot";
                    os << "];\n";
                }
            }

            if (terminal1) {
                os << "  \"$\" [shape=square,label=\"⊤\"];\n";
            }

            os << "  {rank=same; " << i;
            for (size_t j = 0; j < m; ++j) {
                os << "; \"" << NodeId(i, j) << "\"";
            }
            os << "}\n";
        }

        os << "}\n";
        os.flush();
    }
};

/*
该类用于管理节点表格（NodeTableEntity）的处理和引用计数
*/
template<int ARITY>
class NodeTableHandler {
    struct Object {
        // 表示引用计数，用于跟踪有多少个对象共享相同的节点表格
        unsigned refCount;
        // 表示一个节点表格的实例，这是被引用的节点表格。
        NodeTableEntity<ARITY> entity;

        Object(int n)
                : refCount(1), entity(n) {
        }

        Object(NodeTableEntity<ARITY> const& entity)
                : refCount(1), entity(entity) {
        }
        /*
        用于增加引用计数，表示有一个额外的引用指向这个对象。
        如果引用计数达到0，抛出异常以防止过多的引用。
        */
        void ref() {
            ++refCount;
            if (refCount == 0) throw std::runtime_error("Too many references");
        }
        /*
        用于减少引用计数，表示有一个引用停止指向这个对象。
        如果引用计数达到0，删除对象自身。
        */
        void deref() {
            --refCount;
            if (refCount == 0) delete this;
        }
    };

    Object* pointer;

public:
    NodeTableHandler(int n = 1)
            : pointer(new Object(n)) {
    }

    /*
    复制构造函数：创建一个 NodeTableHandler 对象，接受另一个 NodeTableHandler 对象 o 作为参数。
    它会共享 o 中的节点表格，并将引用计数 refCount 增加1。
    */
    NodeTableHandler(NodeTableHandler const& o)
            : pointer(o.pointer) {
        pointer->ref();
    }

    /*
    赋值运算符重载：用于将一个 NodeTableHandler 对象的值赋给另一个对象。
    在赋值过程中，会减少原始对象的引用计数并增加新对象的引用计数，以确保引用计数的正确维护。
    */
    NodeTableHandler& operator=(NodeTableHandler const& o) {
        pointer->deref();
        pointer = o.pointer;
        pointer->ref();
        return *this;
    }

    ~NodeTableHandler() {
        pointer->deref();
    }

    NodeTableEntity<ARITY> const& operator*() const {
        return pointer->entity;
    }

    NodeTableEntity<ARITY> const* operator->() const {
        return &pointer->entity;
    }

    /**
     * Make the table unshared.
     * @return writable reference to the private table.
     */
    /*
    将节点表格设置为不共享的状态，并返回对节点表格的可写引用。如果节点表格被多个对象共享，
    则会创建一个新的 Object 对象来存储一个新的节点表格的拷贝，以确保修改不会影响其他对象。
    */
    NodeTableEntity<ARITY>& privateEntity() {
        if (pointer->refCount >= 2) {
            pointer->deref();
            pointer = new Object(pointer->entity);
        }
        return pointer->entity;
    }

    /**
     * Clear and initialize the table.
     * @param n the number of rows.
     * @return writable reference to the private table.
     */
    /*
    清除并初始化节点表格。如果节点表格不被共享（即引用计数为1），则直接初始化表格；
    否则，会创建一个新的 Object 对象以容纳新的节点表格。
    */
    NodeTableEntity<ARITY>& init(int n = 1) {
        if (pointer->refCount == 1) {
            pointer->entity.init(n);
        }
        else {
            pointer->deref();
            pointer = new Object(n);
        }
        return pointer->entity;
    }

    /**
     * Clear a row if it is not shared.
     * @param i row index.
     */
    void derefLevel(int i) {
        if (pointer->refCount == 1) pointer->entity[i].clear();
    }
};

}// namespace tdzdd
