# MXRuby

mruby wrapper in Elixir

## Usage

```elixir
mruby = MXRuby.create()

{:ok, "foo"} = MXRuby.exec(mruby, """
  class Foo
    def foo
      123
    end
  end
""")

{:error, "NoMethodError: undefined method 'bar'"} = MXRuby.exec(mruby, "Foo.new.bar")

{:ok, "123"} = MXRuby.exec(mruby, "Foo.new.foo")

{:ok, "4"} = MXRuby.exec(mruby, "1 + 3")
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

