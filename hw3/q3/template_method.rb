class Teacher
  def initialize
    raise NotImplementedError if instance_of?(Teacher)

    puts "Initializing teacher #{self.class}"
  end

  def teach
    lecture_a
    short_break
    lecture_b
  end

  def lecture_a
    raise NotImplementedError
  end

  def short_break
    raise NotImplementedError
  end

  def lecture_b
    raise NotImplementedError
  end
end

class Zhao < Teacher
  def lecture_a
    puts 'Lecturing about code smells'
  end

  def short_break
    puts 'Playing video about home country'
  end

  def lecture_b
    puts 'Lecturing about design patterns'
  end
end

zhao = Zhao.new
zhao.teach
