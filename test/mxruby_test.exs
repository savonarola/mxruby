defmodule MXRubyTest do
  use ExUnit.Case
  doctest MXRuby

  test "greets the world" do
    assert MXRuby.hello() == :world
  end
end
