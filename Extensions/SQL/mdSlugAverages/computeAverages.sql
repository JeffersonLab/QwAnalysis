SELECT 
    temp_in.slope_correction,
    temp_in.value_in as VAL_in, temp_in.error_in as ERR_in,
    temp_out.value_out as VAL_out, temp_out.error_out as ERR_out,
    (temp_in.value_in+temp_out.value_out)/2 as NUL,
    
    SQRT(temp_in.error_in * temp_in.error_in + temp_out.error_out * temp_out.error_out)/2 as NUL_Error,
    
    (-temp_in.value_in/temp_in.error_in/temp_in.error_in + 
        temp_out.value_out/temp_out.error_out/temp_out.error_out) /
     (1/temp_in.error_in/temp_in.error_in + 1/temp_out.error_out/temp_out.error_out) AS PHYS,
    
    1/SQRT(1/temp_in.error_in/temp_in.error_in + 1/temp_out.error_out/temp_out.error_out) as PHYS_error
FROM
    temp_in
JOIN 
    temp_out
ON
    temp_in.slope_correction = temp_out.slope_correction
GROUP BY
    slope_correction
