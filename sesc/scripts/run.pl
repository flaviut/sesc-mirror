#!/usr/bin/env perl

# TODO: Check that vortex input and word directories are copied to the local path

use strict;

use Getopt::Long;
use Time::localtime;
use Env;

my $BHOME;

my $op_ext;
my $op_help=0;
my $op_test=0;
my $op_dump=0;
my $op_sesc="$ENV{'SESCBUILDDIR'}/sesc";
my $op_key;
my $op_load=1;
my $op_mload=2;
my $op_clean;
my $op_bhome=$ENV{'BENCHDIR'};
my $op_prof;
my $op_vprof;
my $op_profsec;
my $op_c="$ENV{'SESCBUILDDIR'}/sesc.conf";
my $op_fast;
my $op_data="ref";
my $op_numprocs=1;
my $op_rabbit;
my $op_saveoutput;
my $op_bindir="$ENV{'BENCHDIR'}/bin";
my $op_marks2; 


my $dataset;

my $result = GetOptions("test",\$op_test,
			"dump",\$op_dump,
			"sesc=s",\$op_sesc,
			"key=s",\$op_key,
			"ext=s",\$op_ext,
			"load=i",\$op_load,
			"mload=i",\$op_mload,
			"bhome=s",\$op_bhome,
			"fast",\$op_fast,
			"vprof",\$op_vprof,
			"prof=i",\$op_prof,
			"profsec=s",\$op_profsec,
			"clean",\$op_clean,
			"c=s",\$op_c,
                        "data=s",\$op_data,
			"procs=i",\$op_numprocs,
                        "rabbit", \$op_rabbit,
			"saveoutput",\$op_saveoutput,
			"bindir=s",\$op_bindir,
			"marks2",\$op_marks2,
			"help",\$op_help
		       );


my $threadsRunning=0;

sub waitUntilLoad {
  my $load= shift;

  my $loadavg;

  while (1) {
    open(FH, "uptime|") or die ('Failed to open file');
    while (<FH>) {
      chop();
      # First occurence of loadavg
      if (/(\d+\.\d+)/ ) {
	$loadavg = $1;
	last;
      }
    }
    close(FH);
    last if ($loadavg <= $load);
    print "Load too high (${loadavg} waiting...\n";
    sleep 73;
  }
}

sub getExec {
  my $benchName = shift;

  return "${op_bindir}/${benchName}${op_ext}";
}

sub getMarks {
  my $fastMarks = shift;
  my $normalMarks = shift;

  return $normalMarks if ($op_vprof);

  return "" if (defined($op_rabbit) and !defined($op_prof));

  return $fastMarks if ($op_fast or defined($op_prof));

  return $normalMarks;
}

sub runIt {
  my $prg = shift;

  print RUNLOG "Launching: ";
  print RUNLOG "(Testing) " if( $op_test);
  print RUNLOG $prg . "\n";

  waitUntilLoad($op_mload);

  if( $threadsRunning >= $op_load ) {
    print RUNLOG "Maximum load reached. Waiting for someone to complete\n";
    wait();
    $threadsRunning--;
  }
  if( $op_load == 1 ) {
    if( $op_dump ) {
      print "[$prg]\n";
    }else{
      system($prg);
    }
  }elsif( fork() == 0 ) {
    # Child
    if( $op_dump ) {
      print "[$prg]\n";
    }else{
      system($prg);
    }
    exit(0);
  }else{
    $threadsRunning++;
  }
}

sub runBench {
  my %param = @_;

  die if( !defined($param{bench}) );

  my $sesc = $op_sesc;

  $sesc .= " -x" . $param{xtra} if( defined($param{xtra}) );
  $sesc .= " -t" if( $op_test );
  $sesc .= " -c" . $op_c if( defined $op_c );
  $sesc .= " -w1" if ($op_rabbit or $op_vprof);
  $sesc .= " -r" . $op_prof if (defined $op_prof);
  $sesc .= " -S" . $op_profsec if (defined $op_profsec);

  my $output;
  
  if( $op_saveoutput ) {
    if ( defined($op_key) ) {
      $output = "&> ${param{bench}}${op_ext}.${op_key}.out";
    } else {
      $output = "&> ${param{bench}}${op_ext}.out";
    }
  }

  my $executable = getExec($param{bench});

  ############################################################
  # CINT2000
  ############################################################
  if( $param{bench} eq 'gzip' ) {
    # Comments: Very homogeneous missrate across marks
    my $marks = getMarks("-120 -227", "-14 -2170");
    my $gzipinput;
    if ($op_data eq 'test') {
      $gzipinput = "input.compressed 2";
    } elsif ($op_data eq 'train') {
      $gzipinput = "input.combined 32";
    } else {
      $gzipinput = "input.source 60";
    }
    runIt("${sesc} -h0xF000000 ${marks} ${executable} ${BHOME}/CINT2000/164.gzip/${dataset}/${gzipinput} ${output}"); 

  }elsif( $param{bench} eq 'vpr' ) {
    my $marks = getMarks("-11 -22", "-11 -22");
    my $opt = "${BHOME}/CINT2000/175.vpr/${dataset}/net.in ${BHOME}/CINT2000/175.vpr/${dataset}/arch.in ";
    $opt .= "place.out dum.out -nodisp -place_only -init_t 5 -exit_t 0.005 ";
    $opt .= "-alpha_t 0.9412 -inner_num 2";
    runIt("${sesc} -h0xc00000 ${marks} ${executable} ${opt} ${output}");

  }elsif( $param{bench} eq 'gcc' ) {
    my $marks = getMarks("-11 -25", "-11 -217");
    runIt("${sesc} -k0x80000 -h0x1c00000 ${marks} ${executable} ${BHOME}/CINT2000/176.gcc/${dataset}/200.i -o gcc1.s ${output}");
  }elsif( $param{bench} eq 'mcf' ) {
    my $marks = getMarks("-11 -26", "-11 -250");
    runIt("${sesc} -h0x6000000 ${marks} ${executable} ${BHOME}/CINT2000/181.mcf/${dataset}/inp.in ${output}");

  }elsif( $param{bench} eq 'crafty' ) {
    my $marks = getMarks("-18 -212", "-18 -215");
    runIt("${sesc} -h0x800000 ${marks} ${executable} <${BHOME}/CINT2000/186.crafty/${dataset}/crafty.in ${output}");

  }elsif( $param{bench} eq 'parser' ) {
    my $common= "${BHOME}/CINT2000/197.parser/data/all/input/2.1.dict -batch";
    my $marks = getMarks("-135 -238", "-133 -238");
    runIt("${sesc} -k0x80000 -h0x3C00000 ${marks} ${executable} ${common} <${BHOME}/CINT2000/197.parser/${dataset}/${op_data}.in ${output}");

  }elsif( $param{bench} eq 'gap' ) {
    my $marks = getMarks("-1122 -2123", "-1114 -2126");
    my $opt = "-l ${BHOME}/CINT2000/254.gap/data/all/input  -q -n -m ";
    if ($op_data eq 'test') {
      $opt .= "64M";
    } elsif ($op_data eq 'train') {
      $opt .= "128M";
    } else {
      $opt .= "128M";
    }
    runIt("${sesc} -h0xC000000 ${marks} ${executable} ${opt} <${BHOME}/CINT2000/254.gap/${dataset}/${op_data}.in ${output}");

  }elsif( $param{bench} eq 'vortex' ) {
    my $marks;
    if ($op_marks2) {
	$marks = getMarks("-11 -22", "-11 -22");
    }else{
	$marks = getMarks("-13 -24", "-13 -24");
    }
    my $input;
    # prepare input data then run
    if ($op_data eq 'ref') {
      system("cp ${BHOME}/CINT2000/255.vortex/data/ref/input/persons.1k .");
      system("cp ${BHOME}/CINT2000/255.vortex/data/ref/input/bendian.rnv .");
      system("cp ${BHOME}/CINT2000/255.vortex/data/ref/input/bendian.wnv .");
      $input = "bendian2.raw";
    }elsif ($op_data eq 'train') {
      system("cp ${BHOME}/CINT2000/255.vortex/data/train/input/persons.250 .");
      system("cp ${BHOME}/CINT2000/255.vortex/data/train/input/bendian.rnv .");
      system("cp ${BHOME}/CINT2000/255.vortex/data/train/input/bendian.wnv .");
      $input = "bendian.raw";
    }elsif ($op_data eq 'test') {
      system("cp ${BHOME}/CINT2000/255.vortex/data/test/input/persons.1k .");
      system("cp ${BHOME}/CINT2000/255.vortex/data/test/input/bendian.rnv .");
      system("cp ${BHOME}/CINT2000/255.vortex/data/test/input/bendian.wnv .");
      $input = "bendian.raw";
    }
    runIt("${sesc} -h0x8000000 ${marks} ${executable} ${BHOME}/CINT2000/255.vortex/${dataset}/${input} ${output}");

  }elsif( $param{bench} eq 'bzip2' ) {
    my $marks;
    if ($op_marks2) {
      $marks = getMarks("-13 -26", "-11 -29");
    }else{
      $marks = getMarks("-11 -22", "-11 -22");
    }
    my $input;
    if ($op_data eq 'test') {
      $input = "input.random 2";
    } elsif ($op_data eq 'train') {
      $input = "input.compressed 8";
    } else {
      $input = "input.source 58";
    }
    runIt("${sesc} -h0xbc00000 ${marks} ${executable} ${BHOME}/CINT2000/256.bzip2/${dataset}/${input} ${output}");

  }elsif( $param{bench} eq 'twolf' ) {
    my $marks = getMarks("-12 -23", "-12 -23");
    runIt("${sesc} -h0x8000000 ${marks} ${executable} ${op_data} ${output}");

  }elsif( $param{bench} eq 'perlbmk' ) {
    my $marks = getMarks("-11 -22", "-11 -22");
    my $opt;
    if ($op_data eq 'ref') {
      system("cp ${BHOME}/CINT2000/253.perlbmk/data/all/input/lenums .");
      $opt = "-Ilib ${BHOME}/CINT2000/253.perlbmk/data/all/input/diffmail.pl 2 550 15 24 23 100"
    }
    runIt("${sesc} -h0x8000000 ${marks} ${executable} ${opt} ${output}");

  }
####################################################################################
#  SPEC FP
####################################################################################
  elsif( $param{bench} eq 'wupwise' ) {
    my $marks = getMarks("-11 -22", "-11 -24");
    system("cp ${BHOME}/CFP2000/168.wupwise/${dataset}/wupwise.in .");
    runIt("${sesc} -h0xbc00000 ${marks} ${executable} ${output}");
  }elsif( $param{bench} eq 'swim' ) {
    my $marks = getMarks("-12 -24", "-12 -26");
    runIt("${sesc} -h0xbc00000 ${marks} ${executable} <${BHOME}/CFP2000/171.swim/${dataset}/swim.in ${output}");

  }elsif( $param{bench} eq 'mgrid' ) {
    my $marks = getMarks("-11 -22", "-11 -23");
    runIt("${sesc} -h0xbc00000 ${marks} ${executable} <${BHOME}/CFP2000/172.mgrid/${dataset}/mgrid.in ${output}");

  }elsif( $param{bench} eq 'applu' ) {
    my $marks = getMarks("-11 -2110", "-11 -2220");
    runIt("${sesc} -h0xb000000  -k0x20000 ${marks} ${executable} <${BHOME}/CFP2000/173.applu/${dataset}/applu.in ${output}");

  }elsif( $param{bench} eq 'mesa' ) {
    my $params;
    my $marks = getMarks("-11 -22", "-11 -23");
    system("cp ${BHOME}/CFP2000/177.mesa/${dataset}/numbers .");
    if ($op_data eq 'test') {
      $params = "-frames 10 -meshfile ${BHOME}/CFP2000/177.mesa/${dataset}/mesa.in -ppmfile mesa.ppm ";
    } elsif ($op_data eq 'train') {
      $params = "-frames 500 -meshfile ${BHOME}/CFP2000/177.mesa/${dataset}/mesa.in -ppmfile mesa.ppm";
    } elsif ($op_data eq 'ref') {
      $params = "-frames 1000 -meshfile ${BHOME}/CFP2000/177.mesa/${dataset}/mesa.in -ppmfile mesa.ppm";
    }
    runIt("${sesc} -k0x80000  -h0x8000000 ${marks} ${executable} ${params} ${output}");

  }elsif( $param{bench} eq 'galgel' ) {
    # still needs checking - all input sets
    runIt("echo \" facerec: benchmark in f90, need to find parameters. run may be wrong\"");
    runIt("${sesc} -h0xbc00000 ${executable} <${BHOME}/CFP2000/178.galgel/${dataset}/galgel.in ${output}");

  }elsif( $param{bench} eq 'art' ) {
    my $params;
    my $marks = getMarks("-12 -23", "-12 -25");
    if ($op_data eq 'test') {
      $params = "-stride 2 -startx 134 -starty 220 -endx 139 -endy 225 -objects 1";
    } elsif ($op_data eq 'train') {
      $params = "-stride 2 -startx 134 -starty 220 -endx 184 -endy 240 -objects 3";
    } elsif ($op_data eq 'ref') {
      $params = "-trainfile2 ${BHOME}/CFP2000/179.art/${dataset}/hc.img";
      $params = "-stride 2 -startx 110 -starty 200 -endx 160 -endy 240 -objects 10";
      # --OR-- (spec runs both)
      # $params .= " -stride 2 -startx 470 -starty 140 -endx 520 -endy 180 -objects 10";
    }
    runIt("${sesc} -h0xc000000 ${marks} ${executable} -scanfile ${BHOME}/CFP2000/179.art/${dataset}/c756hel.in -trainfile1 ${BHOME}/CFP2000/179.art/${dataset}/a10.img ${params} ${output}");

  }elsif( $param{bench} eq 'equake' ) {
    my $marks = getMarks("-13 -25", "-13 -213");
    runIt("${sesc} -h0xbc00000 ${marks} ${executable} <${BHOME}/CFP2000/183.equake/${dataset}/inp.in ${output}");

  }elsif( $param{bench} eq 'facerec' ) {
    # still needs checking - all input sets
    runIt("echo \" facerec: benchmark in f90, need to find parameters\"");

  }elsif( $param{bench} eq 'ammp' ) {
    my $marks = getMarks("-1133 -210000", "-1133 -222650");
    if ($op_data eq 'test') {
      system("cp ${BHOME}/CFP2000/188.ammp/${dataset}/all.init.ammp .");
      system("cp ${BHOME}/CFP2000/188.ammp/${dataset}/short.tether .");
    } elsif ($op_data eq 'train') {
      system("cp ${BHOME}/CFP2000/188.ammp/${dataset}/all.new.ammp .");
      system("cp ${BHOME}/CFP2000/188.ammp/${dataset}/new.tether .");
    } elsif ($op_data eq 'ref') {
      system("cp ${BHOME}/CFP2000/188.ammp/${dataset}/all.init.ammp .");
      system("cp ${BHOME}/CFP2000/188.ammp/${dataset}/init_cond.run.1 .");
      system("cp ${BHOME}/CFP2000/188.ammp/${dataset}/init_cond.run.2 .");
      system("cp ${BHOME}/CFP2000/188.ammp/${dataset}/init_cond.run.3 .");
    }
    runIt("${sesc} -h0xc000000 ${marks} ${executable} <${BHOME}/CFP2000/188.ammp/${dataset}/ammp.in ${output}");

  }elsif( $param{bench} eq 'lucas' ) {
    # still needs checking - all input sets
    runIt("echo \" lucas: benchmark in f90, need to find parameters\"");

  }elsif( $param{bench} eq 'fma3d' ) {
    # still needs checking - all input sets
    runIt("echo \" fma3d: benchmark in f90, need to find parameters\"");

  }elsif( $param{bench} eq 'sixtrack' ) {
    system("cp ${BHOME}/CFP2000/200.sixtrack/data/all/input/fort.2 .");
    system("cp ${BHOME}/CFP2000/200.sixtrack/${dataset}/fort.3 .");
    system("cp ${BHOME}/CFP2000/200.sixtrack/${dataset}/fort.7 .");
    system("cp ${BHOME}/CFP2000/200.sixtrack/${dataset}/fort.8 .");
    system("cp ${BHOME}/CFP2000/200.sixtrack/data/all/input/fort.16 .");
    runIt("${sesc} -k0x80000 -h0x8000000 ${executable} <${BHOME}/CFP2000/200.sixtrack/${dataset}/inp.in ${output}");

  }elsif( $param{bench} eq 'apsi' ) {
    system("cp ${BHOME}/CFP2000/301.apsi/${dataset}/apsi.in .");
    runIt("${sesc} -h0xbc00000 ${executable} ${output}");

  }

  ###########################################################
  #  splash2
  ###########################################################

  elsif( $param{bench} eq 'cholesky' ) {
    my $params;
    if ($op_data eq 'ref') {
      $params = "-p${op_numprocs} ${BHOME}/splash2/kernels/cholesky/${dataset}/tk29.O"
    } elsif ($op_data eq 'test') {
      $params = "-p${op_numprocs} ${BHOME}/splash2/kernels/cholesky/${dataset}/lshp.O"
    }
    runIt("${sesc} -h0x8000000 -k0x80000 ${executable} ${params}");

  }elsif( $param{bench} eq 'fft' ) {
    my $params;
    if ($op_data eq 'ref') {
      $params = "-m16 -l5 -p${op_numprocs}";
    } elsif ($op_data eq 'test') {
      $params = "-m12 -l5 -p${op_numprocs}";
    }
    runIt("${sesc} -h0x8000000 ${executable} ${params}");

 }elsif( $param{bench} eq 'lu' ) {
   my $params;
   if ($op_data eq 'ref') {
     $params = "-n512 -b16 -p${op_numprocs}";
   } elsif ($op_data eq 'test') {
     $params = "-n32 -b8 -p${op_numprocs}";
   }
   runIt("${sesc} -h0x8000000 ${executable} ${params}");

  }elsif( $param{bench} eq 'radix' ) {
    my $params;
    if ($op_data eq 'ref') {
      $params = "-r1024 -n1048576 -p${op_numprocs}";
    } elsif ($op_data eq 'test') {
      $params = "-r32 -n65536 -p${op_numprocs}";
    }
    runIt("${sesc} -h0x8000000 ${executable} ${params}"); 

  }elsif( $param{bench} eq 'barnes' ) {
    runIt("${sesc} -h0x8000000 ${executable} < ${BHOME}/splash2/apps/barnes/${dataset}/i${op_numprocs}");

  }elsif( $param{bench} eq 'fmm' ) {
   runIt("${sesc} -h0x8000000 ${executable} < ${BHOME}/splash2/apps/fmm/${dataset}/i16kp${op_numprocs}");

  }elsif( $param{bench} eq 'ocean' ) {
    my $params;
    if ( $op_data eq 'ref') {
      $params = "-n258 -p${op_numprocs}";
    } else {
      $params = "-n34 -p${op_numprocs}";
    }
    runIt("${sesc} -h0x8000000 ${executable} ${params}");

  }elsif( $param{bench} eq 'radiosity' ) {
    my $params;
    if ($op_data eq 'ref') {
      $params = "-p ${op_numprocs} -batch -room"
    } elsif ($op_data eq 'test') {
      $params = "-room -ae 5000.0 -en 0.050 -bf 0.10 -p ${op_numprocs}";
    }
    runIt("${sesc} -h0x8000000 ${executable} ${params}");

  }elsif( $param{bench} eq 'raytrace' ) {
    my $params;
    if ($op_data eq 'ref') {
      system("cp ${BHOME}/splash2/apps/raytrace/${dataset}/car.geo .");
      $params = "-p${op_numprocs} ${BHOME}/splash2/apps/raytrace/${dataset}/car.env";
    } elsif ($op_data eq 'test') {
      system("cp ${BHOME}/splash2/apps/raytrace/${dataset}/teapot.geo .");
      $params = "-p${op_numprocs} ${BHOME}/splash2/apps/raytrace/${dataset}/teapot.env";
    }
    runIt("${sesc} -h0x8000000 ${executable} ${params}");

  }elsif( $param{bench} eq 'volrend' ) {
    my $params;
    if ($op_data eq 'ref') {
      $params = "${op_numprocs} ${BHOME}/splash2/apps/volrend/${dataset}/head";
    } elsif($op_data eq 'test') {
      $params = "${op_numprocs} ${BHOME}/splash2/apps/volrend/${dataset}/head-scaleddown4";
    }
    runIt("${sesc} -h0x8000000 ${executable} ${params}");

  }elsif( $param{bench} eq 'water-nsquared' ) {
    system("cp ${BHOME}/splash2/apps/water-nsquared/${dataset}/random.in .");
    runIt("${sesc} -h0x8000000 ${executable} <${BHOME}/splash2/apps/water-nsquared/${dataset}/input_${op_numprocs}");

  }elsif( $param{bench} eq 'water-spatial' ) {
    system("cp ${BHOME}/splash2/apps/water-spatial/${dataset}/random.in .");
    runIt("${sesc} -h0x8000000 ${executable} <${BHOME}/splash2/apps/water-spatial/${dataset}/input_${op_numprocs}");

  }else{
    die("Unknown benchmark [$param{xtra}]");
  }
}

sub processParams {
  my $badparams =0;

  $badparams = 1 if( @ARGV < 1 );

  if ( -f "./${op_sesc}" ) {
     $op_sesc = "./${op_sesc}";
  }else{
     $op_sesc = $op_sesc;
  }

  if( $op_clean ) {
    print "Do you really want to DELETE all the files? (y/N)";
    my $c = getc();
    if( $c eq 'y' ) {
      unlink <core.*>;
      unlink <sesc_*\.??????>;
      unlink <prof_*>;
      unlink 'run.pl.log';
      unlink 'game.001';
      unlink <crafty*>;
      unlink <AP*>;
      unlink 'mcf.out';
      unlink 'smred.msg';
      unlink 'smred.out';
      unlink 'lgred.msg';
      unlink 'lgred.out';
      unlink 'bendian.rnv';
      unlink 'bendian.wnv';
      unlink 'persons.1k';
      unlink 'vortex1.out';
      unlink 'vortex.msg';
      unlink <input/*> ;
      rmdir 'input' ;
      unlink <words/CVS/*>;
      rmdir 'words/CVS';
      unlink <words/*>;
      rmdir 'words' ;
      unlink 'wupwise.in';
      unlink 'SWIM7';
      unlink 'costs.out';
      unlink '166.i';
      unlink '200.i';
      unlink 'place.out';
      unlink 'all.init.ammp';
      unlink 'init_cond.run.1';
      unlink 'init_cond.run.2';
      unlink 'init_cond.run.3';
      unlink 'a10.img';
      unlink 'c756hel.in';
      unlink 'hc.img';

      unlink 'numbers';
      unlink 'mesa.in';
      unlink 'mesa.log';
      unlink 'mesa.ppm';

      system("rm -rf lgred");
      system("rm -rf smred");
      unlink 'benums' ;

      unlink 'gcc1.s';
      unlink 'apsi.in';

      unlink <test.*>;
      unlink <train.*>;
      unlink <ref.*>;
    }
    exit 0;
  }

  if( defined ($op_bhome) ) {
      $BHOME = $op_bhome;
  }else{
      print "Path to benchmarks directory undefined. Use bhome option\n";
      $badparams =1;
  }

  if( !defined ($op_bindir) ) {
      print "Path to benchmarks binary undefined. Use bindir option\n";
      $badparams =1;
  }

  if ($op_data eq "test" || $op_data eq "ref" || $op_data eq "train") {
    $dataset  = "data/";
    $dataset .= $op_data;
    $dataset .= "/input";
  } else {
    print "Choose dataset to run: [test|ref|train]\n";
    $badparams =1;
  }

  if( $badparams or $op_help ){
    print "usage:\n\trun.pl <options> <benchs>*\n";
    print "CINT2000: crafty mcf parser gzip vpr bzip2 gcc gap twolf vortex perlbmk\n";
    print "CFP2000 : wupwise swim mgrid applu apsi equake ammp art mesa\n";
    print "SPLASH2: cholesky fft lu radix barnes fmm ocean radiosity  \n";
    print "         raytrace volrend water-nsquared water-spatial     \n";
    print "Misc   : mp3dec mp3enc smatrix\n";
    print "\t-sesc=s          ; Simulator executable (default is $ENV{'SESCBUILDDIR'}/sesc)\n";
    print "\t-c=s             ; Configuration file (default is $op_c).\n";
    print "\t-bhome=s         ; Path for the benchmarks directory is located (default is $op_bhome)\n";
    print "\t-bindir=s        ; Specify where the benchmarks binaries are (deafult is $op_bindir).\n";
    print "\t-marks2          ; Choose a different set of simulation marks (Only for testing, not official marks).\n";
    print "\t-ext=s           ; Extension to be added to the binary names\n";
    print "\t-data=s          ; Data set [test|train|ref]\n";
    print "\t-test            ; Just test the configuration\n";
    print "\t-key=s           ; Extra key added to result file name\n";
    print "\t-load=i          ; # simultaneous simulations running\n";
    print "\t-mload=i         ; Maximum machine load\n";
    print "\t-prof=i          ; Profiling run\n";
    print "\t-profsec=s       ; Profiling section\n";
    print "\t-fast            ; Shorter marks for fast test\n";
    print "\t-procs=i         ; Number of threads in a splash application\n";
    print "\t-rabbit          ; Run in Rabbit mode the whole program\n";
    print "\t-saveoutput      ; Save output to a .out file\n";
    print "\t-clean           ; DELETE all the outputs from previous runs\n";
    print "\t-help            ; Show this help\n";
    exit;
  }
}

sub setupDirectory {
  unless( -f "words") {
    system("cp -r ${BHOME}/CINT2000/197.parser/data/all/input/words .")
  }

  system("cp ${BHOME}/CINT2000/300.twolf/${dataset}/${op_data}.* .");
  system("ln -sf ${BHOME}/CINT2000/253.perlbmk/data/all/input/lib lib");
}

exit &main();

###########

sub main {

  processParams();

  my $tmp;
  my $bench;

  open( RUNLOG, ">>run.pl.log") or die("Can't open log file");

  print RUNLOG "-----------------------------------------\n";
  print RUNLOG ctime(time()) . "   :   ";
  print RUNLOG `uname -a`;
  print RUNLOG "-----------------------------------------\n";

  setupDirectory();

  foreach $bench (@ARGV) {
    runBench( bench => $bench , xtra => $op_key);
  }

  print "RUN: Spawning finished. Waiting for results...\n" if( $threadsRunning );

  while( $threadsRunning > 0 ) {
    wait();
    $threadsRunning--;
    print "RUN: Another finished. $threadsRunning to complete\n";
  }

  close(RUNLOG);
}
