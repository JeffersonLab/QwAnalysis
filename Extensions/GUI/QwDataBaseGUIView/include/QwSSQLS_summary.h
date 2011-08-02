
#ifndef QWSSQLS_SUMMARY_HH
#define QWSSQLS_SUMMARY_HH

// Need to include define EXPAND_MY_SSQLS_STATICS in owner module
//
#if !defined(EXPAND_MY_SSQLS_STATICS)
#define MYSQLPP_SSQLS_NO_STATICS
#endif

#include <mysql++.h>
#include <ssqls.h>

namespace QwParityDB{


    sql_create_20(summary_by, 1, 2
      , mysqlpp::sql_int_unsigned , summary_by_id
			, mysqlpp::sql_int_unsigned , run_number
      , mysqlpp::Null<mysqlpp::sql_tinyint_unsigned> , segment_number
      , mysqlpp::Null<mysqlpp::sql_enum> , slow_helicity_plate
      , mysqlpp::Null<mysqlpp::sql_enum> , wien_reversal
      , mysqlpp::sql_int_unsigned , n
      , mysqlpp::sql_tinyint_unsigned , subblock
			, mysqlpp::Null<mysqlpp::sql_float> , q_value
			, mysqlpp::Null<mysqlpp::sql_float> , q_error
			, mysqlpp::Null<mysqlpp::sql_float> , x_value
			, mysqlpp::Null<mysqlpp::sql_float> , x_error
			, mysqlpp::Null<mysqlpp::sql_float> , y_value
			, mysqlpp::Null<mysqlpp::sql_float> , y_error
			, mysqlpp::Null<mysqlpp::sql_float> , theta_x_value
			, mysqlpp::Null<mysqlpp::sql_float> , theta_x_error
			, mysqlpp::Null<mysqlpp::sql_float> , theta_y_value
			, mysqlpp::Null<mysqlpp::sql_float> , theta_y_error
			, mysqlpp::Null<mysqlpp::sql_float> , energy_value
			, mysqlpp::Null<mysqlpp::sql_float> , energy_error
			, mysqlpp::sql_int_unsigned , run_quality_id
		) 
  
    sql_create_20(summary_ba, 1, 2
      , mysqlpp::sql_int_unsigned , summary_ba_id
			, mysqlpp::sql_int_unsigned , run_number
      , mysqlpp::Null<mysqlpp::sql_tinyint_unsigned> , segment_number
      , mysqlpp::Null<mysqlpp::sql_enum> , slow_helicity_plate
      , mysqlpp::Null<mysqlpp::sql_enum> , wien_reversal
      , mysqlpp::sql_int_unsigned , n
      , mysqlpp::sql_tinyint_unsigned , subblock
			, mysqlpp::Null<mysqlpp::sql_float> , q_value
			, mysqlpp::Null<mysqlpp::sql_float> , q_error
			, mysqlpp::Null<mysqlpp::sql_float> , x_value
			, mysqlpp::Null<mysqlpp::sql_float> , x_error
			, mysqlpp::Null<mysqlpp::sql_float> , y_value
			, mysqlpp::Null<mysqlpp::sql_float> , y_error
			, mysqlpp::Null<mysqlpp::sql_float> , theta_x_value
			, mysqlpp::Null<mysqlpp::sql_float> , theta_x_error
			, mysqlpp::Null<mysqlpp::sql_float> , theta_y_value
			, mysqlpp::Null<mysqlpp::sql_float> , theta_y_error
			, mysqlpp::Null<mysqlpp::sql_float> , energy_value
			, mysqlpp::Null<mysqlpp::sql_float> , energy_error
			, mysqlpp::sql_int_unsigned , run_quality_id
		) 
  
    sql_create_11(summary_dy_calc, 1, 2
			, mysqlpp::sql_int_unsigned , summary_dy_calc_id
			, mysqlpp::sql_int_unsigned , run_number
      , mysqlpp::Null<mysqlpp::sql_tinyint_unsigned> , segment_number
      , mysqlpp::Null<mysqlpp::sql_enum> , slow_helicity_plate
      , mysqlpp::Null<mysqlpp::sql_enum> , wien_reversal
      , mysqlpp::sql_int_unsigned , n
      , mysqlpp::sql_tinyint_unsigned , subblock
			, mysqlpp::Null<mysqlpp::sql_text> , detector
			, mysqlpp::Null<mysqlpp::sql_float> , value
			, mysqlpp::Null<mysqlpp::sql_float> , error
			, mysqlpp::sql_int_unsigned , run_quality_id
		) 
  
    sql_create_11(summary_dy_corr, 1, 2
			, mysqlpp::sql_int_unsigned , summary_dy_corr_id
			, mysqlpp::sql_int_unsigned , run_number
      , mysqlpp::Null<mysqlpp::sql_tinyint_unsigned> , segment_number
      , mysqlpp::Null<mysqlpp::sql_enum> , slow_helicity_plate
      , mysqlpp::Null<mysqlpp::sql_enum> , wien_reversal
      , mysqlpp::sql_int_unsigned , n
      , mysqlpp::sql_tinyint_unsigned , subblock
			, mysqlpp::Null<mysqlpp::sql_text> , detector
			, mysqlpp::Null<mysqlpp::sql_float> , value
			, mysqlpp::Null<mysqlpp::sql_float> , error
			, mysqlpp::sql_int_unsigned , run_quality_id
		) 
  
    sql_create_11(summary_da_calc, 1, 2
			, mysqlpp::sql_int_unsigned , summary_da_calc_id
			, mysqlpp::sql_int_unsigned , run_number
      , mysqlpp::Null<mysqlpp::sql_tinyint_unsigned> , segment_number
      , mysqlpp::Null<mysqlpp::sql_enum> , slow_helicity_plate
      , mysqlpp::Null<mysqlpp::sql_enum> , wien_reversal
      , mysqlpp::sql_int_unsigned , n
      , mysqlpp::sql_tinyint_unsigned , subblock
			, mysqlpp::Null<mysqlpp::sql_text> , detector
			, mysqlpp::Null<mysqlpp::sql_float> , value
			, mysqlpp::Null<mysqlpp::sql_float> , error
			, mysqlpp::sql_int_unsigned , run_quality_id
		) 
  
    sql_create_11(summary_da_corr, 1, 2
			, mysqlpp::sql_int_unsigned , summary_da_corr_id
			, mysqlpp::sql_int_unsigned , run_number
      , mysqlpp::Null<mysqlpp::sql_tinyint_unsigned> , segment_number
      , mysqlpp::Null<mysqlpp::sql_enum> , slow_helicity_plate
      , mysqlpp::Null<mysqlpp::sql_enum> , wien_reversal
      , mysqlpp::sql_int_unsigned , n
      , mysqlpp::sql_tinyint_unsigned , subblock
			, mysqlpp::Null<mysqlpp::sql_text> , detector
			, mysqlpp::Null<mysqlpp::sql_float> , value
			, mysqlpp::Null<mysqlpp::sql_float> , error
			, mysqlpp::sql_int_unsigned , run_quality_id
		) 
  
    }
#endif
