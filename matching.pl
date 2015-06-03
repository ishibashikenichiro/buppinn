use strict;
use warnings;
 
my $imageDir = "./pic";
&mainSub();
 
sub loadHist(){
    my %hist;
    open(FP, "<histograms.txt");
    foreach my $line (<FP>){
        chomp($line);
        my @data = split(/\t/, $line);
        my $file =  shift(@data);
        my @data2;
        foreach my $d (@data){
            if($d eq "-nan" || $d eq "nan"){
                push ( @data2, 0 );
            }else{
                push ( @data2, $d );
            }
        }
        @{$hist{$file}} = @data2;
 
    }
    close(FP);
    return %hist;
}
 
sub calcHistIntersection(){
    my ($h1, $h2) = @_;
    my @hist1 = @$h1;
    my @hist2 = @$h2;
    my $total = 0;
    for ( my $i = 0; $i < $#hist1; $i++ ){
        $total += min($hist1[$i], $hist2[$i]);
    }
    return $total / sum(@hist1);
}
 
sub min(){
    my @nums = @_;
    if ( $nums[0] < $nums[1]){
        return $nums[0];
    }else{
        return $nums[1];
    }
}
 
sub sum(){
    my @nums = @_;
    my $total = 0;
    foreach my $num (@nums){
        $total += $num;
    }
    return $total;
}
 
sub mainSub(){
    my %hist = &loadHist();
    while ( 1 ){
        my $queryFile = <STDIN>;
        chomp($queryFile);
        if ( "quit" eq $queryFile ){
            last;
        }
        $queryFile = $imageDir . "/" . $queryFile;
        unless ( exists($hist{$queryFile})){
            print "no histogram\n";
            next;
        }
        my %result;
        my @queryHist = @{$hist{$queryFile}};
        while ( my ($key, $h) = each(%hist)) {
            my @targetHist = @{$h};
            my $d = &calcHistIntersection(\@queryHist, \@targetHist);
            $result{$key} = $d;
        }
        for my $key (sort {$result{$b} <=> $result{$a} || $a cmp $b} keys %result) {
            print $result{$key} . "\t" . $key . "\n";
        }
    }
}
