defmodule FibBench do
  use Benchfella


  setup_all do
    fib = """
      def fib(n)
        return 1 if n <= 1
        return fib(n - 1) + fib(n - 2)
      end
    """

    mrb = MXRuby.new()
    MXRuby.eval(mrb, fib)
    {:ok, mrb}
  end

  before_each_bench mrb do
    {:ok, mrb}
  end


  def fib_elixir(n) when n <= 1, do: 1
  def fib_elixir(n) do
    fib_elixir(n - 1) + fib_elixir(n - 2)
  end

  bench "fib_mruby"  do
    MXRuby.eval(bench_context, "fib($mxruby[:val].to_i)", val: to_string(35))
  end

  bench "fib_elixir" do
    fib_elixir(35)
  end

end

