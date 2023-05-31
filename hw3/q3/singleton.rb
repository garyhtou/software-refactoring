require 'singleton'

class MonaLisa
  include Singleton

  def name
    'The Mona Lisa'
  end

  def painter
    'Leonardo da Vinci'
  end

  def year
    '1503'
  end

  def location
    'The Louvre'
  end
end

mona = MonaLisa.instance
puts "#{mona.name} was painted by #{mona.painter} in #{mona.year}."
lisa = MonaLisa.instance
puts "It is currently located at #{lisa.location}."
puts "There is only one instance of The Mona Lisa: #{mona.object_id} == #{lisa.object_id}"
