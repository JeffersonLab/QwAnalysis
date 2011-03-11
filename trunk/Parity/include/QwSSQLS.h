#ifndef QWSSQLS_HH
#define QWSSQLS_HH

#if (__GNUC__ == 4) && (__GNUC_MINOR__ > 5)
#  pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wvariadic-macros"

// Need to include define EXPAND_MY_SSQLS_STATICS in owner module
//
#if !defined(EXPAND_MY_SSQLS_STATICS)
#define MYSQLPP_SSQLS_NO_STATICS
#endif

#include <mysql++.h>
#include <ssqls.h>

namespace QwParityDB {


    sql_create_6(db_schema, 1, 2 
      , mysqlpp::sql_int_unsigned , db_schema_id
      , mysqlpp::sql_char , major_release_number
      , mysqlpp::sql_char , minor_release_number
      , mysqlpp::sql_char , point_release_number
      , mysqlpp::sql_timestamp , time
      , mysqlpp::Null<mysqlpp::sql_text> , script_name
    ) 
  
    sql_create_2(good_for, 1, 2 
			, mysqlpp::sql_int_unsigned , good_for_id
			, mysqlpp::Null<mysqlpp::sql_text> , type
		) 
  
    sql_create_2(run_quality, 1, 2 
			, mysqlpp::sql_int_unsigned , run_quality_id
			, mysqlpp::Null<mysqlpp::sql_text> , type
		) 
  
    sql_create_9(run, 1, 2 
			, mysqlpp::sql_int_unsigned , run_id
			, mysqlpp::sql_int_unsigned , run_number 	
      , mysqlpp::sql_int_unsigned , slug
			, mysqlpp::Null<mysqlpp::sql_text> , run_type
//			, mysqlpp::Null<mysqlpp::sql_set> , good_for_id
//			, mysqlpp::Null<mysqlpp::sql_set> , run_quality_id
			, mysqlpp::Null<mysqlpp::sql_datetime> , start_time
			, mysqlpp::Null<mysqlpp::sql_datetime> , end_time
			, mysqlpp::sql_int_unsigned , n_mps
			, mysqlpp::sql_int_unsigned , n_qrt	
			, mysqlpp::Null<mysqlpp::sql_text> , comment
		) 
  
    sql_create_9(runlet, 1, 2 
      , mysqlpp::sql_int_unsigned , runlet_id
      , mysqlpp::sql_int_unsigned , run_id
      , mysqlpp::sql_int_unsigned , run_number
      , mysqlpp::Null<mysqlpp::sql_int_unsigned> , segment_number
      , mysqlpp::sql_enum , full_run
			, mysqlpp::Null<mysqlpp::sql_datetime> , start_time
			, mysqlpp::Null<mysqlpp::sql_datetime> , end_time
      , mysqlpp::sql_int_unsigned , first_mps
      , mysqlpp::sql_int_unsigned , last_mps
  ) 
  
    sql_create_16(analysis, 1, 2 
			, mysqlpp::sql_int_unsigned , analysis_id
			, mysqlpp::sql_int_unsigned , runlet_id
			, mysqlpp::sql_int_unsigned , seed_id
			, mysqlpp::sql_int_unsigned , monitor_calibration_id
			, mysqlpp::sql_int_unsigned , cut_id
			, mysqlpp::Null<mysqlpp::sql_datetime> , time
			, mysqlpp::Null<mysqlpp::sql_text> , bf_checksum
			, mysqlpp::sql_enum , beam_mode
			, mysqlpp::sql_int_unsigned , n_mps
			, mysqlpp::sql_int_unsigned , n_qrt
			, mysqlpp::Null<mysqlpp::sql_int_unsigned> , first_event
			, mysqlpp::Null<mysqlpp::sql_int_unsigned> , last_event
			, mysqlpp::Null<mysqlpp::sql_int> , segment
			, mysqlpp::Null<mysqlpp::sql_enum> , slope_calculation
			, mysqlpp::Null<mysqlpp::sql_enum> , slope_correction
      , mysqlpp::Null<mysqlpp::sql_enum> , root_based_regression
		) 
  
    sql_create_7(md_slope, 1, 2 
			, mysqlpp::sql_int_unsigned , md_slope_id
			, mysqlpp::sql_int_unsigned , analysis_id
			, mysqlpp::sql_int_unsigned , slope_type_id
			, mysqlpp::sql_char , measurement_type_id
			, mysqlpp::sql_int_unsigned , main_detector_id
			, mysqlpp::sql_float , value
			, mysqlpp::sql_float , error
		) 
  
    sql_create_7(lumi_slope, 1, 2 
			, mysqlpp::sql_int_unsigned , lumi_slope_id
			, mysqlpp::sql_int_unsigned , analysis_id
			, mysqlpp::sql_int_unsigned , slope_type_id
			, mysqlpp::sql_char , measurement_type_id
			, mysqlpp::sql_int_unsigned , lumi_detector_id
			, mysqlpp::sql_float , value
			, mysqlpp::sql_float , error
		) 
  
    sql_create_8(beam, 1, 2 
			, mysqlpp::sql_int_unsigned , beam_id
			, mysqlpp::sql_int_unsigned , analysis_id	
			, mysqlpp::sql_int_unsigned , monitor_id
			, mysqlpp::sql_char , measurement_type_id
      , mysqlpp::sql_tinyint_unsigned , subblock
			, mysqlpp::sql_int_unsigned , n
			, mysqlpp::sql_float , value
			, mysqlpp::sql_float , error	
		) 
  
    sql_create_5(cut, 1, 2 
			, mysqlpp::sql_int_unsigned , cut_id
			, mysqlpp::sql_int_unsigned , start_run_id
			, mysqlpp::sql_int_unsigned , end_run_id
			, mysqlpp::Null<mysqlpp::sql_datetime> , time
			, mysqlpp::Null<mysqlpp::sql_text> , comment
		) 
  
    sql_create_5(cut_data, 1, 2 
			, mysqlpp::sql_int_unsigned , cut_data_id
			, mysqlpp::sql_int_unsigned , cut_id
			, mysqlpp::sql_int_unsigned , monitor_id
			, mysqlpp::sql_float , min
			, mysqlpp::sql_float , max
		) 
  
    sql_create_3(measurement_type, 1, 2 
			, mysqlpp::sql_char , measurement_type_id
			, mysqlpp::sql_text , units
			, mysqlpp::sql_text , title
		) 
  
    sql_create_4(slope_type, 1, 2 
			, mysqlpp::sql_int_unsigned , slope_type_id
			, mysqlpp::sql_text , slope
			, mysqlpp::sql_text , units
			, mysqlpp::sql_text , title
		) 
  
    sql_create_3(monitor, 1, 2 
			, mysqlpp::sql_int_unsigned , monitor_id
			, mysqlpp::sql_text , quantity
			, mysqlpp::sql_text , title
		) 
  
    sql_create_3(main_detector, 1, 2 
      , mysqlpp::sql_int_unsigned , main_detector_id
      , mysqlpp::sql_text , quantity
      , mysqlpp::sql_text , title
    ) 
  
    sql_create_8(md_data, 1, 2 
			, mysqlpp::sql_int_unsigned , md_data_id
			, mysqlpp::sql_int_unsigned , analysis_id
			, mysqlpp::Null<mysqlpp::sql_int_unsigned> , main_detector_id
			, mysqlpp::sql_char , measurement_type_id
      , mysqlpp::sql_tinyint_unsigned , subblock
			, mysqlpp::sql_int_unsigned , n
			, mysqlpp::sql_float , value
			, mysqlpp::sql_float , error
		) 
  
    sql_create_3(lumi_detector, 1, 2 
      , mysqlpp::sql_int_unsigned , lumi_detector_id
      , mysqlpp::sql_text , quantity
      , mysqlpp::sql_text , title
    ) 
  
    sql_create_8(lumi_data, 1, 2 
			, mysqlpp::sql_int_unsigned , lumi_data_id
			, mysqlpp::sql_int_unsigned , analysis_id
			, mysqlpp::Null<mysqlpp::sql_int_unsigned> , lumi_detector_id
			, mysqlpp::sql_char , measurement_type_id
      , mysqlpp::sql_tinyint_unsigned , subblock
			, mysqlpp::sql_int_unsigned , n
			, mysqlpp::sql_float , value
			, mysqlpp::sql_float , error
		) 
  
    sql_create_9(slow_controls_settings, 1, 2 
			, mysqlpp::sql_int_unsigned , slow_controls_settings_id
			, mysqlpp::sql_int_unsigned , runlet_id
      , mysqlpp::Null<mysqlpp::sql_enum> , slow_helicity_plate
      , mysqlpp::Null<mysqlpp::sql_enum> , wien_reversal
      , mysqlpp::Null<mysqlpp::sql_int_unsigned> , helicity_length
      , mysqlpp::Null<mysqlpp::sql_enum> , charge_feedback
      , mysqlpp::Null<mysqlpp::sql_enum> , position_feedback
      , mysqlpp::Null<mysqlpp::sql_float> , qtor_current
      , mysqlpp::Null<mysqlpp::sql_text> , target_position
    ) 
  
    sql_create_4(sc_detector, 1, 2 
			, mysqlpp::sql_int_unsigned , sc_detector_id
      , mysqlpp::sql_text , name
      , mysqlpp::sql_text , units
      , mysqlpp::sql_text , comment
    ) 
  
    sql_create_7(slow_controls_data, 1, 2 
			, mysqlpp::sql_int_unsigned , slow_controls_data_id
			, mysqlpp::sql_int_unsigned , runlet_id
      , mysqlpp::sql_int_unsigned , sc_detector_id
      , mysqlpp::sql_float , value
      , mysqlpp::sql_float , error
      , mysqlpp::sql_float , min_value
      , mysqlpp::sql_float , max_value
    ) 
  
    sql_create_4(slow_controls_strings, 1, 2 
			, mysqlpp::sql_int_unsigned , slow_controls_strings_id
			, mysqlpp::sql_int_unsigned , runlet_id
      , mysqlpp::sql_int_unsigned , sc_detector_id
      , mysqlpp::sql_text , value
    ) 
  
    sql_create_3(pmt, 1, 2 
			, mysqlpp::sql_int_unsigned , pmt_id
			, mysqlpp::sql_int_unsigned , detector_id
			, mysqlpp::Null<mysqlpp::sql_enum> , position
		) 
  
    sql_create_5(monitor_calibration, 1, 2 
			, mysqlpp::sql_int_unsigned , monitor_calibration_id
			, mysqlpp::sql_int_unsigned , first_run_id
			, mysqlpp::sql_int_unsigned , last_run_id
			, mysqlpp::Null<mysqlpp::sql_datetime> , time
			, mysqlpp::Null<mysqlpp::sql_text> , comment
		) 
  
    sql_create_5(monitor_calibration_data, 1, 2 
			, mysqlpp::sql_int_unsigned , monitor_calibration_data_id
			, mysqlpp::sql_int_unsigned , monitor_calibration_id
			, mysqlpp::sql_int_unsigned , monitor_id
			, mysqlpp::sql_float , gain
			, mysqlpp::sql_float , offset
		) 
  
    sql_create_5(seeds, 1, 2 
			, mysqlpp::sql_int_unsigned , seed_id
      , mysqlpp::sql_int_unsigned , first_run_id
      , mysqlpp::sql_int_unsigned , last_run_id
			, mysqlpp::Null<mysqlpp::sql_text> , seed
			, mysqlpp::Null<mysqlpp::sql_text> , comment
		) 
  
    sql_create_4(bf_test, 1, 2 
			, mysqlpp::sql_int_unsigned , bf_test_id
			, mysqlpp::sql_int_unsigned , analysis_id
			, mysqlpp::Null<mysqlpp::sql_int_unsigned> , test_number
			, mysqlpp::Null<mysqlpp::sql_float> , test_value
		) 
  
    sql_create_2(high_voltage_file, 1, 2 
			, mysqlpp::sql_int_unsigned , high_voltage_file_id
			, mysqlpp::Null<mysqlpp::sql_text> , name
		) 
  
    sql_create_5(high_voltage_setting, 1, 2 
			, mysqlpp::sql_int_unsigned , high_voltage_setting_id
			, mysqlpp::sql_int_unsigned , high_voltage_file_id
			, mysqlpp::sql_int_unsigned , pmt_id
			, mysqlpp::sql_float , setting
			, mysqlpp::sql_float , gain
		) 
  
    sql_create_3(high_voltage, 1, 2 
			, mysqlpp::sql_int_unsigned , high_voltage_id
			, mysqlpp::sql_int_unsigned , runlet_id
			, mysqlpp::sql_int_unsigned , high_voltage_file_id
		) 
  
    sql_create_5(high_voltage_reading, 1, 2 
			, mysqlpp::sql_int_unsigned , high_voltage_reading_id
			, mysqlpp::sql_int_unsigned , high_voltage_id
			, mysqlpp::sql_int_unsigned , pmt_id
			, mysqlpp::sql_float , value
			, mysqlpp::sql_float , error
		) 
  
    sql_create_6(compton_run, 1, 2 
			, mysqlpp::sql_int_unsigned , compton_run_id
			, mysqlpp::Null<mysqlpp::sql_int_unsigned> , run_number
			, mysqlpp::Null<mysqlpp::sql_datetime> , start_time
			, mysqlpp::Null<mysqlpp::sql_datetime> , end_time
			, mysqlpp::sql_int_unsigned , n_mps
			, mysqlpp::sql_int_unsigned , n_qrt
		) 
  
    sql_create_6(compton_data, 1, 2 
			, mysqlpp::sql_int_unsigned , compton_data_id
			, mysqlpp::sql_int_unsigned , compton_run_id
			, mysqlpp::sql_char , measurement_type_id
			, mysqlpp::Null<mysqlpp::sql_enum> , particle_type
			, mysqlpp::sql_float , value
			, mysqlpp::sql_float , error
		) 
  
    }


#if (__GNUC__ == 4) && (__GNUC_MINOR__ > 5)
#  pragma GCC diagnostic pop
#endif

#endif
