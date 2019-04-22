defmodule MXRubyTest do
  use ExUnit.Case
  doctest MXRuby

  test "eval" do
    fib = """
      def calc(n)
        return 1 if n <= 1
        return calc(n - 1) + calc(n - 2)
      end
    """

    mrb = MXRuby.new()
    MXRuby.eval(mrb, fib)

    assert {:ok, "10946"} = MXRuby.eval(mrb, "calc($mxruby[:val].to_i)", val: "20")
  end
end
