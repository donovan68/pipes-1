#pragma once

#include <functional>

#include "pipes2/detail/util/metaprogramming.hpp"
#include "pipes2/detail/util/is_pushable.hpp"

#include "pipes2/detail/core/OutputIteratorBase.hpp"

namespace tillh::pipes2
{
  template<class Op, class Outputs>
  class Output : public OutputIteratorBase<Output<Op, Outputs>>
  {
    static_assert(std::is_copy_constructible_v<Op>);
    static_assert(std::is_copy_assignable_v<Op>);
    static_assert(std::is_swappable_v<Op>);
    static_assert(std::is_destructible_v<Op>);
  public:
    template<class ... Outputs_>
    Output(Op op, const std::tuple<Outputs_...>& outputs)
      : op_(std::move(op)),
      outputs_(outputs)
    {}

    template<class T>
    void push(T&& t) const
    {
      if constexpr(std::tuple_size_v<Outputs> == 0)
      {
        op_.push(std::forward<T>(t));
      }
      else
      {
        std::apply([this, &t](auto& ... sinks) { op_.push(std::forward<T>(t), sinks...); }, outputs_);
      }
    }

  private:
    Op op_;
    Outputs outputs_;
  };
}

namespace tillh::pipes2
{
  template<class Op, class Outputs>
  auto makeOutput(Op op, Outputs outputs)
  {
    return Output<Op, Outputs>(op, outputs);
  }
}

namespace tillh::pipes2
{
  template<class T>
  struct is_output : std::false_type {};

  template<class Op, class Outputs>
  struct is_output<Output<Op, Outputs>> : std::true_type {};

  template<class T>
  constexpr static bool is_output_v = is_output<T>::value;
}
