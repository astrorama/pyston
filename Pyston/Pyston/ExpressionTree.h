/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef PYSTON_EXPRESSIONTREE_H
#define PYSTON_EXPRESSIONTREE_H

#include <functional>
#include "Pyston/Graph/Node.h"

namespace Pyston {

/*
 * Declaration to allow for function-like templates
 */
template<typename Signature>
class ExpressionTree;

/**
 * Wraps a call to an expression tree
 * @tparam R
 *  Return type
 * @tparam Args
 *  Argument types
 */
template<typename R, typename ...Args>
class ExpressionTree<R(Args...)> {
public:

  /**
   * Use the tree as a function
   * @param args
   *    Argument list
   */
  R operator()(const Context& context, Args&&...args) {
    return m_root->eval(context, std::forward<Args>(args)...);
  }

  R operator()(Args&& ...args) {
    return m_root->eval(Context{}, std::forward<Args>(args)...);
  }

  /**
   * @return
   *    True if the expression tree has been fully "compiled",
   *    false if there is a wrapped call to Python under the hood
   */
  bool isCompiled() const {
    return m_is_compiled;
  }

  /**
   * @return
   *    The root of the expression tree
   */
  const std::shared_ptr<Node<R>>& getTree() const {
    return m_root;
  }

private:
  bool m_is_compiled;
  std::shared_ptr<Node<R>> m_root;

  ExpressionTree(bool compiled, const std::shared_ptr<Node<R>>& root)
    : m_is_compiled(compiled), m_root(root) {
  }

  friend class ExpressionTreeBuilder;
};

} // end of namespace Pyston

#endif //PYSTON_EXPRESSIONTREE_H
