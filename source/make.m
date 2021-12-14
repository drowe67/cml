% makefile for the CML mex files
%
% Last updated July 4, 2006

% first clear all the mex files
clear Capacity.cpp
clear CapacityTableLookup.cpp
clear ConvEncode.cpp
clear CreateSRandomInterleaver.cpp
clear CreateUmtsInterleaver.cpp
clear CreateCcsdsInterleaver.cpp
clear Deinterleave.cpp
clear Depuncture.cpp
clear InitializeDVBS2.cpp
clear Interleave.cpp
clear LdpcEncode.cpp
clear MpDecode.cpp
clear Puncture.cpp
clear RateDematch.cpp
clear RateMatch.cpp
clear SisoDecode.cpp
clear Somap.cpp
clear ViterbiDecode.cpp
clear Modulate.cpp
clear DemodFSK.cpp
clear Demod2D.cpp

% if this is linux, the files need to be deleted
if isunix
    delete ../mex/*
end

% determine version
tempstring = version;
if (( strcmp(tempstring(1:3), '7.2' )|strcmp(tempstring(1:3), '7.1' ) )&ispc) % version 7.1 or 7.2
    mex -output ../mex/Capacity.dll Capacity.cpp
    mex -output ../mex/CapacityTableLookup.dll CapacityTableLookup.cpp
    mex -output ../mex/ConvEncode.dll ConvEncode.cpp
    mex -output ../mex/CreateSRandomInterleaver.dll CreateSRandomInterleaver.cpp
    mex -output ../mex/CreateUmtsInterleaver.dll CreateUmtsInterleaver.cpp
    mex -output ../mex/CreateCcsdsInterleaver.dll CreateCcsdsInterleaver.cpp
    mex -output ../mex/Deinterleave.dll Deinterleave.cpp
    mex -output ../mex/Depuncture.dll Depuncture.cpp
    mex -output ../mex/InitializeDVBS2.dll InitializeDVBS2.cpp
    mex -output ../mex/Interleave.dll Interleave.cpp
    mex -output ../mex/LdpcEncode.dll LdpcEncode.cpp
    mex -output ../mex/MpDecode.dll MpDecode.cpp
    mex -output ../mex/Puncture.dll Puncture.cpp
    mex -output ../mex/RateDematch.dll RateDematch.cpp
    mex -output ../mex/RateMatch.dll RateMatch.cpp
    mex -output ../mex/SisoDecode.dll SisoDecode.cpp
    mex -output ../mex/Somap.dll Somap.cpp
    mex -output ../mex/ViterbiDecode.dll ViterbiDecode.cpp
    mex -output ../mex/Modulate.dll Modulate.cpp
    mex -output ../mex/DemodFSK.dll DemodFSK.cpp
    mex -output ../mex/Demod2D.dll Demod2D.cpp
else
    mex -output ../mex/Capacity Capacity.cpp
    mex -output ../mex/CapacityTableLookup CapacityTableLookup.cpp
    mex -output ../mex/ConvEncode ConvEncode.cpp
    mex -output ../mex/CreateSRandomInterleaver CreateSRandomInterleaver.cpp
    mex -output ../mex/CreateUmtsInterleaver CreateUmtsInterleaver.cpp
    mex -output ../mex/CreateCcsdsInterleaver CreateCcsdsInterleaver.cpp
    mex -output ../mex/Deinterleave Deinterleave.cpp
    mex -output ../mex/Depuncture Depuncture.cpp
    mex -output ../mex/InitializeDVBS2 InitializeDVBS2.cpp
    mex -output ../mex/Interleave Interleave.cpp
    mex -output ../mex/LdpcEncode LdpcEncode.cpp
    mex -output ../mex/MpDecode MpDecode.cpp
    mex -output ../mex/Puncture Puncture.cpp
    mex -output ../mex/RateDematch RateDematch.cpp
    mex -output ../mex/RateMatch RateMatch.cpp
    mex -output ../mex/SisoDecode SisoDecode.cpp
    mex -output ../mex/Somap Somap.cpp
    mex -output ../mex/ViterbiDecode ViterbiDecode.cpp
    mex -output ../mex/Modulate Modulate.cpp
    mex -output ../mex/DemodFSK DemodFSK.cpp
    mex -output ../mex/Demod2D Demod2D.cpp
end
