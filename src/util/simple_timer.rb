require_relative './debug'

class SimpleTimer
  def initialize(execute = false)
    @start = {}
    start if execute
  end

  def start(id = 'none')
    @start[id] = SimpleTimer.time_ms
  end

  def stop(id = 'none')
    SimpleTimer.time_ms - @start[id]
  end

  def self.time_ms
    Time.now.instance_eval { self.to_i * 1000 + (usec/1000) }
  end
end
