% test GrB_assign and GxB_subassign
clear all
make
addpath ('~/ssget') ;
addpath ('spok') ;

for nth = [2 1]
    nthreads_set (nth) ;

    for d = 0 % [1 0]
        if (d)
            debug_on
        else
            debug_off
        end
        gb

        test07
        test07b

        test08
        test08b

        test09
        test09b

        test106

        testc7
        test29

        test30
        test30b
        test39

        test46
        test46b

        test51
        test51b

        test54

        test55
        test55b

        test64
        test64b

        test69
        test83
        test84
        test97

        test19
        test19b
        test21
        test21b

    end
end