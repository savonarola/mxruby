defmodule Mix.Tasks.Compile.MxRuby do
  def run(_args) do
    {result, _errcode} = System.cmd("make", [], stderr_to_stdout: true)
    IO.puts(result)
  end
end


defmodule MXRuby.MixProject do
  use Mix.Project

  def project do
    [
      app: :mxruby,
      compilers: [:mx_ruby] ++ Mix.compilers,
      version: "0.1.0",
      elixir: "~> 1.6",
      build_embedded: Mix.env == :prod,
      start_permanent: Mix.env() == :prod,
      deps: deps()
    ]
  end

  def application do
    [
      extra_applications: [:logger]
    ]
  end

  defp deps do
    [{:benchfella, "~> 0.3.0", only: :dev}]
  end
end
