ERL_INCLUDE_PATH=$(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)

FLAGS=-O3

UNAME=$(shell uname)

ifeq ($(UNAME),Linux)
	FLAGS += -shared
endif

ifeq ($(UNAME),Darwin)
	FLAGS += -dynamiclib -undefined dynamic_lookup
endif

RAKE=./minirake
MRUBY_ROOT=mruby
MRUBY_INCLUDE=$(MRUBY_ROOT)/include
MRUBY_A=$(MRUBY_ROOT)/build/host/lib/libmruby.a

MRUBY_CONFIG ?= ../build_config.rb

all: build_mruby build_mxruby

clean: clean_mxruby clean_mruby

build_mxruby: priv/mxruby.so

priv/mxruby.so: c_src/mxruby.c
	mkdir -p priv
	cc -fPIC -I$(ERL_INCLUDE_PATH) -I$(MRUBY_INCLUDE) $(FLAGS) -o priv/mxruby.so c_src/mxruby.c $(MRUBY_A)

build_mruby:
	cd $(MRUBY_ROOT) && $(RAKE) MRUBY_CONFIG=$(MRUBY_CONFIG)

clean_mruby:
	cd $(MRUBY_ROOT) && $(RAKE) deep_clean && rm -rf build

clean_mxruby:
	rm -f priv/mxruby.so

