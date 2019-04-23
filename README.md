# MXRuby

mruby wrapper in Elixir

## Usage

```elixir
mruby = MXRuby.new()

{:ok, "foo"} = MXRuby.eval(mruby, """
  class Foo
    def foo
      123
    end
  end
""")

{:error, "NoMethodError: undefined method 'bar'"} = MXRuby.eval(mruby, "Foo.new.bar")

{:ok, "123"} = MXRuby.eval(mruby, "Foo.new.foo")

{:ok, "4"} = MXRuby.eval(mruby, "1 + 3")
```

## Passing arguments

```elixir
mruby = MXRuby.new()
{:ok, res} = MXRuby.eval(mruby, "$mxruby.inspect", arg1: "val1", arg2: "val2")
IO.puts(res)
```

Output:

```
{:arg1=>"val1", :arg2=>"val2"}
```

Argument keyword list should have only binaries as values.

## `mruby` customisation

```elixir

use Mix.Config

config :mxruby, :build_params, """
  toolchain :gcc
  conf.cc.flags << %w{-fPIC -O3}
  conf.gembox 'full-core'

  conf.gem :github => 'matsumotory/mruby-simplehttp'
  conf.gem :github => 'matsumotory/mruby-httprequest'
  conf.gem :github => 'mattn/mruby-http'
  conf.gem :github => 'mattn/mruby-json'
"""

```

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed
by adding `mxruby` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:mxruby, "~> 0.1.0"}
  ]
end
```

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc)
and published on [HexDocs](https://hexdocs.pm). Once published, the docs can
be found at [https://hexdocs.pm/mxruby](https://hexdocs.pm/mxruby).

