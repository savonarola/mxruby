defmodule MXRuby do
  @moduledoc false

  @on_load :load_nifs

  def load_nifs do
    path = Application.app_dir(:mxruby, "priv/mxruby")
    :ok = :erlang.load_nif(path, 0)
  end

  def exec(mrb, code) when is_binary(code) do
    exec_nif(mrb, code)
  end

  def create() do
    create_nif()
  end

  def exec_nif(_mrb, _code) do
    raise "NIF exec_nif/2 not implemented"
  end

  def create_nif() do
    raise "NIF create_nif/0 not implemented"
  end

end
