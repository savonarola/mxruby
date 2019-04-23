MRuby::Build.new('host') do |conf|
  toolchain :gcc

  conf.cc.flags << %w{-fPIC -O3}

  conf.gembox 'full-core'
end
