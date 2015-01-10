
% Copyright (c) 2002, 2015 Jens Keiner, Stefan Kunis, Daniel Potts
%
% This program is free software; you can redistribute it and/or modify it under
% the terms of the GNU General Public License as published by the Free Software
% Foundation; either version 2 of the License, or (at your option) any later
% version.
%
% This program is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
% FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
% details.
%
% You should have received a copy of the GNU General Public License along with
% this program; if not, write to the Free Software Foundation, Inc., 51
% Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

% Test script of class nnfft for spatial dimension d=2.
clear all;

M=16; % number of nodes
N_1=8; % number of Fourier coefficients in first direction
N_2=8; % number of Fourier coefficients in second direction
N=[N_1;N_2];
N1_1=2*N_1;
N1_2=2*N_2;
N1=[N1_1;N1_2];
N_total=N_1*N_2;

x=rand(M,2)-0.5; %nodes
v=rand(N_total,2)-0.5; %nodes

% Initialisation
%plan=nnfft(2,N_total,M,N); % create plan of class type nnfft

plan=nnfft(2,N_total,M,N,N1,N1,6,'PRE_PHI_HUT'); % use of nnfft_init_guru

plan.x=x; % set nodes in plan
plan.v=v; % set nodes in plan
nnfft_precompute_psi(plan); % precomputations

% NFFT %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

fhat=rand(N_1,N_2); % Fourier coefficients
fhatv=fhat(:);

% Compute samples with NNFFT
plan.fhat=fhatv; % set Fourier coefficients
nnfft_trafo(plan); % compute nonequispaced Fourier transform
f1=plan.f; % get samples

% Compute samples direct
nnfft_trafo_direct(plan);
f2=plan.f; 

% Compare results
disp('NNFFT vs NNDFT');
max(abs(f1-f2))




