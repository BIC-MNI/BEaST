#  beast_normalize.sh
#
#  Copyright 2011  Simon Fristed Eskildsen, Vladimir Fonov,
#   	      	    Pierrick Coupé, Jose V. Manjon
#
#  This file is part of mincbeast.
#
#  mincbeast is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  mincbeast is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with mincbeast.  If not, see <http://www.gnu.org/licenses/>.
#
#  For questions and feedback, please contact:
#  Simon Fristed Eskildsen <eskild@gmail.com> 

#!/bin/bash
num_args=3
usage_string="<input mnc> <output mnc> <output xfm>"
options_string="\t-initial <xfm>\n\t-noreg\n\t-non3\n\t-modeldir <path>\n\t-modelname <name>"

unset list
declare -a list
listcount=0

reg=1
n3=1
initxfm=""
modeldir=$MNI_DATAPATH/icbm152_model_09c
modelname=mni_icbm152_t1_tal_nlin_sym_09c

while [ 0 -eq 0 ]
do
  case "$1" in
  -nor*)
    reg=0
    shift
    ;;
  -non*)
    n3=0
    shift
    ;;
  -ini*)
    initxfm=$2
    shift 2
    ;;
  -modeldir)
    modeldir=$2
    shift 2
    ;;
  -modelname)
    modelname=$2
    shift 2
    ;;
  *)
    if [ "$1" != "" ]; then
	list=( "${list[@]}" "$1" )
	let listcount=listcount+1
	shift
    else
	break
    fi
  esac
done

# get number of entries
max=${#list[*]}

test $max -ne $num_args && echo -e "Usage: `basename $0` $usage_string\nOptions:\n$options_string" && exit 

input=${list[0]}
output=${list[1]}
xfm=${list[2]}

if [ ! -d $modeldir ]; then
    echo "Error!\tCannot find model directory."
    echo "\tEnvironment variable MNI_DATAPATH is empty or -modeldir is not set"
    echo -e "Usage: `basename $0` $usage_string\nOptions:\n$options_string"
    exit
fi

template=$modeldir/$modelname.mnc
mask=$modeldir/${modelname}_mask.mnc

tmp=/tmp/normal_$$
mkdir $tmp

mincreshape -float -normalize $input $tmp/reshaped.mnc

if [ $reg -eq 1 ]; then
# step 1: no mask
    if [ "$initxfm" = "" ]; then
	if [ $n3 -eq 1 ]; then
	    nu_correct -iter 100 -stop 0.0001 -fwhm 0.1 $tmp/reshaped.mnc $tmp/nuc1.mnc
	else
	    ln -s $tmp/reshaped.mnc $tmp/nuc1.mnc
	fi
	volume_pol --order 1 --min 0 --max 100 --noclamp $tmp/nuc1.mnc $template --expfile $tmp/file1.exp
	minccalc $tmp/nuc1.mnc $tmp/normal1.mnc -expfile $tmp/file1.exp -short	
	bestlinreg_s $tmp/normal1.mnc $template $tmp/lin1.xfm
	mincresample -like $template -transform $tmp/lin1.xfm $tmp/normal1.mnc $tmp/final1.mnc	
    else
	cp $initxfm $tmp/lin1.xfm
	ln -s $tmp/reshaped.mnc $tmp/final1.mnc
    fi    
else
    param2xfm $tmp/lin1.xfm
    ln -s $tmp/reshaped.mnc $tmp/final1.mnc
fi

# step 2: with mask
mincresample -invert -nearest -like $tmp/reshaped.mnc -transform $tmp/lin1.xfm $mask $tmp/mask.mnc -clob
if [ $n3 -eq 1 ]; then
    nu_correct -iter 100 -stop 0.0001 -fwhm 0.1 $tmp/reshaped.mnc $tmp/nuc2.mnc -mask $tmp/mask.mnc -clob
else
    ln -s $tmp/reshaped.mnc $tmp/nuc2.mnc
fi
volume_pol --order 1 --min 0 --max 100 --noclamp $tmp/nuc2.mnc $template --expfile $tmp/file2.exp --source_mask $tmp/mask.mnc --target_mask $mask --clobber
minccalc $tmp/nuc2.mnc $tmp/normal2.mnc -expfile $tmp/file2.exp -short -clob
if [ $reg -eq 1 ]; then
    bestlinreg_s $tmp/normal2.mnc $template $tmp/lin2.xfm -source_mask $tmp/mask.mnc -target_mask $mask -clob
    mincresample -like $template -transform $tmp/lin2.xfm $tmp/normal2.mnc $output -sinc -clob
else
    cp $tmp/normal2.mnc $output

fi

cp $tmp/lin1.xfm $xfm

rm -r $tmp

