
#ifndef QWSSQLS_HH
#define QWSSQLS_HH

// Need to include define EXPAND_MY_SSQLS_STATICS in owner module
//
#if !defined(EXPAND_MY_SSQLS_STATICS)
#define MYSQLPP_SSQLS_NO_STATICS
#endif

#include <mysql++.h>
#include <ssqls.h>

namespace QwParityDB{


    sql_create_6(db_schema, 1, 2 
      , mysqlpp::sql_int_unsigned , db_schema_id
      , mysqlpp::sql_char , major_release_number
      , mysqlpp::sql_char , minor_release_number
      , mysqlpp::sql_char , point_release_number
      , mysqlpp::sql_timestamp , time
      , mysqlpp::Null<mysqlpp::sql_text> , script_name
    ) 
  
    sql_create_8(run, 1, 2 
			, mysqlpp::sql_int_unsigned , run_id
			, mysqlpp::sql_int_unsigned , run_number 	
			, mysqlpp::Null<mysqlpp::sql_enum> , run_type
      , mysqlpp::sql_int_unsigned , helicity_length
			, mysqlpp::Null<mysqlpp::sql_datetime> , start_time
			, mysqlpp::Null<mysqlpp::sql_datetime> , end_time
			, mysqlpp::sql_int_unsigned , n_mps
			, mysqlpp::sql_int_unsigned , n_qrt	
		) 
  
    sql_create_15(analysis, 1, 2 
			, mysqlpp::sql_int_unsigned , analysis_id
			, mysqlpp::sql_int_unsigned , run_id
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
		) 
  
    sql_create_6(slope, 1, 2 
			, mysqlpp::sql_int_unsigned , slope_id
			, mysqlpp::sql_int_unsigned , analysis_id
			, mysqlpp::sql_int_unsigned , slope_type_id
			, mysqlpp::sql_int_unsigned , detector_id
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
  
    sql_create_3(seeds, 1, 2 
			, mysqlpp::sql_int_unsigned , seed_id
			, mysqlpp::sql_text , seed
			, mysqlpp::Null<mysqlpp::sql_text> , comment
		) 
  
    sql_create_4(bf_test, 1, 2 
			, mysqlpp::sql_int_unsigned , bf_test_id
			, mysqlpp::sql_int_unsigned , analysis_id
			, mysqlpp::Null<mysqlpp::sql_int_unsigned> , test_number
			, mysqlpp::Null<mysqlpp::sql_float> , test_value
		) 
  
    sql_create_5(polarized_source, 1, 2 
			, mysqlpp::sql_int_unsigned , polarized_source_id
			, mysqlpp::sql_int_unsigned , run_id
			, mysqlpp::Null<mysqlpp::sql_enum> , slow_helicity_plate
			, mysqlpp::Null<mysqlpp::sql_enum> , charge_feedback
			, mysqlpp::sql_int , sign_in_neg
		) 
  
    sql_create_7(polarized_source_measurement, 1, 2 
			, mysqlpp::sql_int_unsigned , polarized_source_measurement_id
			, mysqlpp::sql_int_unsigned , polarized_source_id
			, mysqlpp::sql_int_unsigned , polarized_source_monitor_id
			, mysqlpp::sql_float , average_value
			, mysqlpp::sql_float , error
			, mysqlpp::sql_float , min_value
			, mysqlpp::sql_float , max_value
		) 
  
    sql_create_4(polarized_source_monitor, 1, 2 
			, mysqlpp::sql_int_unsigned , polarized_source_monitor_id
			, mysqlpp::Null<mysqlpp::sql_text> , quantity
			, mysqlpp::Null<mysqlpp::sql_text> , units
			, mysqlpp::Null<mysqlpp::sql_text> , title
		) 
  
    sql_create_3(target, 1, 2 
			, mysqlpp::sql_int_unsigned , target_id
			, mysqlpp::sql_int_unsigned , run_id
			, mysqlpp::Null<mysqlpp::sql_enum> , type
		) 
  
    sql_create_7(target_measurement, 1, 2 
			, mysqlpp::sql_int_unsigned , target_measurements_id
			, mysqlpp::sql_int_unsigned , target_id
			, mysqlpp::sql_int_unsigned , target_monitor_id
			, mysqlpp::sql_float , average_value
			, mysqlpp::sql_float , error
			, mysqlpp::sql_float , min_value
			, mysqlpp::sql_float , max_value
		) 
  
    sql_create_5(target_monitor, 1, 2 
			, mysqlpp::sql_int_unsigned , target_monitor_id
			, mysqlpp::Null<mysqlpp::sql_enum> , type
			, mysqlpp::Null<mysqlpp::sql_text> , quantity
			, mysqlpp::Null<mysqlpp::sql_text> , units
			, mysqlpp::Null<mysqlpp::sql_text> , title
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
			, mysqlpp::sql_int_unsigned , run_id
			, mysqlpp::sql_int_unsigned , high_voltage_file_id
		) 
  
    sql_create_5(high_voltage_reading, 1, 2 
			, mysqlpp::sql_int_unsigned , high_voltage_reading_id
			, mysqlpp::sql_int_unsigned , high_voltage_id
			, mysqlpp::sql_int_unsigned , pmt_id
			, mysqlpp::sql_float , value
			, mysqlpp::sql_float , error
		) 
  
    sql_create_2(qtor, 1, 2 
			, mysqlpp::sql_int_unsigned , qtor_id
			, mysqlpp::sql_int_unsigned , run_id
		) 
  
    sql_create_7(qtor_measurement, 1, 2 
			, mysqlpp::sql_int_unsigned , qtor_measurement_id
			, mysqlpp::sql_int_unsigned , qtor_id
			, mysqlpp::sql_int_unsigned , qtor_monitor_id
			, mysqlpp::sql_float , average_value
			, mysqlpp::sql_float , error
			, mysqlpp::sql_float , min_value
			, mysqlpp::sql_float , max_value
		) 
  
    sql_create_5(qtor_monitor, 1, 2 
			, mysqlpp::sql_int_unsigned , qtor_monitor_id
			, mysqlpp::Null<mysqlpp::sql_enum> , type
			, mysqlpp::Null<mysqlpp::sql_text> , quantity
			, mysqlpp::Null<mysqlpp::sql_text> , title
			, mysqlpp::Null<mysqlpp::sql_text> , units		
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
#endif
