MRuby::Build.new('host') do |conf|
  # load specific toolchain settings

  toolchain :gcc

  conf.cc.flags << %w{-fPIC -O3}

  conf.gembox 'full-core'

  conf.gem :github => 'matsumotory/mruby-simplehttp'
  conf.gem :github => 'matsumotory/mruby-httprequest'
  conf.gem :github => 'matsumotory/mruby-uname'
  conf.gem :github => 'mattn/mruby-http'
  conf.gem :github => 'mattn/mruby-json'
  conf.gem :github => 'iij/mruby-env'

end
