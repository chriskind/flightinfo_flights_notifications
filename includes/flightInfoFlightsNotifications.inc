<?php

/**
 * Builds forms & messages for flight delay messages
 */
class flightInfoFlightsNotifications {
  
  public static $form;
  public static $form_state;
  
  public $flight_id;
  public $flight_information;
  public $rolling_message = '';
  
  public $emails_users = array();
  public $phonenumbers_users = array();
  
  public $emails_admin = array();
  public $emails_devel = array();
  
  public $phonenumbers_admins = array();
  public $phonenumbers_devel = array();
  
  public static $subject;
  public static $sms_subject;
  
  public static $message;
  public static $sms_message;
  
  function __construct($form, $form_state) {
    
    self::$form = $form;
    self::$form_state = $form_state;
    
    // set flight id
    $flight_id = self::$form['field_notification_flight']['und']['#options'];
    next($flight_id);
    $flight_id = key($flight_id);
    $this->flight_id = $flight_id;
    $this->flight_information = flight_information($this->flight_id);
    $this->set_rolling_message();
    self::$form['field_notification_flight']['und']['#default_value'][] = $this->flight_id;
    
    // adhd_buffer_to_dsm($this->flight_information, 'dpm');
    
    // get emails & phone
    $this->set_default_options_emails = self::$form['field_emails_to_notify']['und']['#options'];
    $this->set_default_options_phone_numbers = self::$form['field_phone_numbers_to_notify']['und']['#options'];
    $this->admin_notification_emails_phonenumbers = variable_get('admin_notification_emails_phonenumbers', array());
    $this->devel_notification_emails_phonenumbers = variable_get('devel_notification_emails_phonenumbers', array());
    $this->build_email_phone_options();
    
    // build and add subjects + messages
    $this->build_messages();
    self::$form['field_fln_subject']['und'][0]['value']['#default_value'] = self::$subject;
    self::$form['body']['und'][0]['#default_value'] = self::$message;
    self::$form['field_fln_sms_subject']['und'][0]['value']['#default_value'] = self::$sms_subject;
    self::$form['field_fln_sms_message']['und'][0]['#default_value'] = self::$sms_message;
    
    // adhd_buffer_to_dsm(self::$sms_subject, 'dpm');
    
    $this->add_extra_fields();
    $this->create_confirm_message();
    
    // customize some buttons
    self::$form['actions']['submit']['#value'] = t('SEND NOW');
    self::$form['actions']['submit']['#attributes']['class'][] = 'btn-primary';
    self::$form['#submit'][] = 'flightinfo_flights_notifications_message_submit';
    
  }
  
  function get_form() {
    // return to drupal
    $return = array();
    $return['form'] = self::$form;
    $return['form_state'] = self::$form_state;
    return($return);
  }
  
  function set_rolling_message() {
    if(!empty($this->flight_information['field_flight_rolling_message']['value'])) {
      $this->rolling_message = "*** IMPORTANT FLIGHT DELAY INFO ***\n" . ($this->flight_information['field_flight_rolling_message']['value']) . "\n\n";
    } else {
      $this->rolling_message = '';
    }
  }
  
  function build_messages() {
    
    self::$subject = "SCHEDULE UPDATE FOR: " . $this->flight_information['client_airline']['airline']  . " " . $this->flight_information['client_airline']['flight#'] . " "  . $this->flight_information['from']['from'] . ' (' . $this->flight_information['from']['from_code'] . ") -to- " .  $this->flight_information['to']['to'] . ' (' . $this->flight_information['to']['to_code'] . ')';
    
    // Trim the SMS (& Amazon SNS subject to 100 characters)
    $sms_subject = "SCHEDULE UPDATE: " . $this->flight_information['client_airline']['airline']  . " " . $this->flight_information['client_airline']['flight#'] . " "  . ' (' . $this->flight_information['from']['from_code'] . ") -to- " .  ' (' . $this->flight_information['to']['to_code'] . ')';
    $sms_subject = substr($sms_subject,0,100);
    self::$sms_subject = $sms_subject;    
    
    $this->build_email_message();
    $this->build_sms_message();
  }
  
  
  function build_email_message() {
      
     if(!empty(self::$form_state) && !empty(self::$form_state['values']['body']['und'][0]['value'])) {
       self::$message = self::$form_state['values']['body']['und'][0]['value'];
     } else {
    
      /*
       * Begin multi line "$message" variable
       */
      
       adhd_buffer_to_dsm($this->flight_information['field_flight_rolling_message'], 'dpm');
       
      self::$message = 
      self::$subject . "\n" . "\n" .
          
      //"Rolling Delay: " . $this->flight_information['field_flight_rolling_delay'] . "\n" .
      $this->rolling_message . 
      
      "Departure Status: " . $this->flight_information['schedule']['depart_status'] . "\n" .
      "Scheduled: " . $this->flight_information['schedule']['depart_local'] . "\n" .   
      "Current Estimated: " . $this->flight_information['schedule']['depart_eta'] . "\n" . "\n" .
      
      "Arrival Status " . $this->flight_information['schedule']['arrival_status'] . "\n" .
      "Scheduled: " . $this->flight_information['schedule']['arrival_local'] . "\n" .
      "Current Estimated: " . $this->flight_information['schedule']['arrival_eta'] . "\n". "\n" .
      
      "*This is an information only website, please check the airport monitors or speak with an airline representative for additional information." . "\n". "\n" . 
      
      "This message sent by " . $GLOBALS['base_url'] . ".";
      
      /*
       * End multi line $message variable
       */
    
    }
  }
  
  function build_sms_message() {
       
     if(!empty(self::$form_state) && isset(self::$form_state['values']['field_fln_sms_message']['und'][0]['value'])) {
       self::$sms_message = self::$form_state['values']['field_fln_sms_message']['und'][0]['value'];
     } else {
       
       /*
       * Begin multi line "$message" variable
       */
      self::$sms_message = 
      self::$subject . "\n\n" .
      
      $this->rolling_message . 
          
      "Departure Status: " . $this->flight_information['schedule']['depart_status'] . "\n" .
      "Current Estimated: " . $this->flight_information['schedule']['depart_eta'] . "\n" . "\n" .
      
      "Arrival Status " . $this->flight_information['schedule']['arrival_status'] . "\n" .
      "Current Estimated: " . $this->flight_information['schedule']['arrival_eta'] . "\n". "\n" .
      
      "More Details@ " . $GLOBALS['base_url'] . "/node/$this->flight_id"; 
      
      /*
       * End multi line $sms_message variable
       */
      
    }
  }


  
  function build_email_phone_options() {

    // Create admin email and phone number arrays
    foreach($this->admin_notification_emails_phonenumbers as $key => $value) {
      
      if(strpos($key, 'email_') !== FALSE) {
        if(!empty($value)) {
          $this->emails_admin[] = $value;
        }
      }
      
      if(strpos($key, 'phone_') !== FALSE) {
        if(!empty($value)) {
          $this->phonenumbers_admin[] = $value;
        }
      }

    }
    
    // Create devel email and phone number arrays
    foreach($this->devel_notification_emails_phonenumbers as $key => $value) {
      
      if(strpos($key, 'email_') !== FALSE) {
        if(!empty($value)) {
          $this->emails_devel[] = $value;
        }
      }
      
      if(strpos($key, 'phone_') !== FALSE) {
        if(!empty($value)) {
          $this->phonenumbers_devel[] = $value;
        }
      }

    }
    
     // Create user email and phone number arrays using entity_metadata_wrapper...
    foreach($this->set_default_options_emails as $nid => $value) {
      
      $node = node_load($nid);
      $follower_info = entity_metadata_wrapper('node', $node);
      
      $email = $follower_info->field_anon_address->value();
      
      if(!empty($email)) {
        $this->emails_users[] = $follower_info->field_anon_address->value();
        self::$form['field_emails_to_notify']['und']['#default_value'][] = $nid;
      } else {
        unset(self::$form['field_emails_to_notify']['und']['#options'][$nid]);
      }
    }
    
    
    foreach($this->set_default_options_phone_numbers as $nid => $value) {
      
      $node = node_load($nid);
      $follower_info = entity_metadata_wrapper('node', $node);
      
      $phone_number = $follower_info->field_anon_phone_number->value();
      
      if(!empty($phone_number)) {
        // $cc = $phone_number['country_codes'];
        $prefix = cck_phone_countrycodes($phone_number['country_codes']);
        $prefix = $prefix['code'];
        $aws_phone_num = $prefix . $phone_number['number'];
        
        
        $this->phonenumbers_users[] = $aws_phone_num;
        self::$form['field_phone_numbers_to_notify']['und']['#default_value'][] = $nid;
      } else {
        unset(self::$form['field_phone_numbers_to_notify']['und']['#options'][$nid]);
      }     
    }
  }
  
  function add_extra_fields() {
    self::$form['email_batch_users'] = array(
        '#type' => 'hidden', 
        '#value' => $this->emails_users, 
        // '#default_value' => ''
    );
    
    self::$form['sms_batch_users'] = array(
        '#type' => 'hidden', 
        '#value' => $this->phonenumbers_users, 
        // '#default_value' => ''
    );
    
    self::$form['email_batch_admin'] = array(
        '#type' => 'hidden', 
        '#value' => $this->emails_admin, 
        // '#default_value' => ''
    );
    
    self::$form['sms_batch_admin'] = array(
        '#type' => 'hidden', 
        '#value' => $this->phonenumbers_admin, 
        // '#default_value' => ''
    );
    
    self::$form['email_batch_devel'] = array(
        '#type' => 'hidden', 
        '#value' => $this->emails_devel, 
        // '#default_value' => ''
    );
    
    self::$form['sms_batch_devel'] = array(
        '#type' => 'hidden', 
        '#value' => $this->phonenumbers_devel, 
        // '#default_value' => ''
    );
    
  }

  function create_confirm_message() {
    
    $subject = self::$form_state['values']['field_fln_subject']['und'][0]['value'];
    $message = self::$form_state['values']['body']['und'][0]['value'];
    
    $sms_subject = self::$form_state['values']['field_fln_sms_subject']['und'][0]['value'];
    $sms_message = self::$form_state['values']['field_fln_sms_message']['und'][0]['value'];
    
    $field_fln_confirm_trigger_suffix = self::$form['field_fln_confirm_trigger']['und']['#suffix'];
    $confirmation_text = $this->confirm_message('confirm_message', $subject, $message, $sms_subject, $sms_message);
    self::$form['field_fln_confirm_trigger']['und']['#suffix'] = $confirmation_text . $field_fln_confirm_trigger_suffix;
    self::$form['field_fln_confirm_trigger']['#attributes']['class'] = array();

  }
  
  function confirm_message($template = 'confirm_message', $subject = NULL, $message = NULL, $sms_subject = NULL, $sms_message = NULL) {
    $vars = array(
        'subject' => str_replace("\n", '<br>', $subject),
        'message' => str_replace("\n", '<br>', $message),
        'sms_subject' => str_replace("\n", '<br>', $sms_subject),
        'sms_message' => str_replace("\n", '<br>', $sms_message),
      );
    
    $output = theme('flightinfo_flights_notifications_confirm_message', $vars);
    return $output;
  }
  
 
 
}