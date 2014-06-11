class TestPrinter
  def self.test_start(test_name)
    puts ColoredString.green "############################# #{test_name} #############################"
  end

  def self.test_end
    puts ColoredString.green "##########################################################"
    puts
  end

  def self.test_case_start(test_case_name)
    puts
    puts ColoredString.blue "# #{test_case_name} ---"
  end

  def self.test_case_end
    puts ColoredString.blue "# ---"
    puts
  end

  def self.print_result(result)
    if result == true then puts ColoredString.green result 
    elsif result == false then puts ColoredString.red result 
    else puts result
    end
  end
end
