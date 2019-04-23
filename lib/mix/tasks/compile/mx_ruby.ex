defmodule Mix.Tasks.Compile.MxRuby do
  use Mix.Task
  @shortdoc "Compile mruby"
  @moduledoc false

  @build_params """
    toolchain :gcc
    conf.cc.flags << %w{-fPIC -O3}
    conf.gembox 'full-core'
  """

  @build_config_template """
  MRuby::Build.new('host') do |conf|
  {build_params}
  end
  """

  defp build_params do
    Application.get_env(:mxruby, :build_params, @build_params)
  end

  defp build_config_path do
    :mxruby
    |> :code.priv_dir()
    |> to_string()
    |> Path.join("build_config.rb")
  end

  defp file_changed?(path, new_config) do
    case File.read(path) do
      {:ok, content} ->
        content != new_config
      _ -> true
    end
  end

  defp create_build_config do
    config = String.replace(@build_config_template, "{build_params}", build_params())
    path = build_config_path()
    if file_changed?(path, config) do
      IO.puts("Created new mruby config #{path}")
      File.write!(path, config)
    end
    path
  end

  def run(_args) do
    config = create_build_config()
    IO.puts("Compiling mruby, it may take time...")
    {output, errcode} = System.cmd("make", ["MRUBY_CONFIG=#{config}"])
    if errcode == 0 do
      IO.puts("Compiled mruby successfully")
      :ok
    else
      IO.puts("Failed to compile mruby")
      {:error, [output]}
    end
  end
end


