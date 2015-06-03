
use strict;
use warnings;
 
$imageDir = "./pic";

 
def loadHist()
    hist
    IO.foreach("histograms.txt") do |line|
         @data = $line.split(/\t/)
         $file = @data.shift()
         @data2
        for $d in @data
            if $d==="-nan" || $d==="nan" 
                @data2.push ( 0 )
            else
                @data2.push ( $d )
            end
        end
        $hist[$file] = @data2
 
    end
end

loadHist()
