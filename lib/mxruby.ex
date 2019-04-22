defmodule MXRuby do
  @moduledoc false

  @on_load :load_nifs

  def load_nifs do
    path = Application.app_dir(:mxruby, "priv/mxruby")
    :ok = :erlang.load_nif(path, 0)
  end

  def eval(mrb, code, args \\ []) when is_reference(mrb) and is_binary(code) do
    unless valid_kv?(args) do
      Process.exit(:badarg, "args must be a keyword list with binaries as values")
    end
    exec_nif(mrb, code, args)
  end

  defp valid_kv?([]), do: true
  defp valid_kv?([{k, v} | rest]) when is_atom(k) and is_binary(v), do: valid_kv?(rest)
  defp valid_kv?(_), do: false

  def new() do
    create_nif()
  end

  defp exec_nif(_mrb, _code, _args) do
    raise "NIF exec_nif/3 not implemented"
  end

  defp create_nif() do
    raise "NIF create_nif/0 not implemented"
  end

end
