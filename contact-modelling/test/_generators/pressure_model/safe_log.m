function [ ret ] = safe_log(coeff, logarg)
%safe_log following 4.3 of "Love's problem" paper, this function will
%         return zero for coeff ~ 0 and coeff * log(arg) otherwise
  if eq_almost(coeff, 0)
    ret = 0;
  else
    ret = coeff * log(logarg);
  end
end