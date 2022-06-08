<?php

/**
 * Creates module admin form via class
 * 
 */
class flightInfoFlightsNotificationsAdmin {
  
  function set_admin_notify_email_addresses() {
    
  }
  
  function get_admin_notify_email_addresses() {
    
  }
  
 
  static function get_admin_menu() {
    $flightInfoFlightsNotificationsAdmin = new flightInfoFlightsNotificationsAdmin();
    return $flightInfoFlightsNotificationsAdmin->admin_menu();
  }
    
  function admin_menu() {
    $items = array();
    
    $items['admin/config/flightinfo/notifications'] = array(
        'title' => 'Flightinfo Notifications Admin', 
        'description' => 'Configuration for administrator notifications', 
        'page callback' => 'drupal_get_form', 
        'page arguments' => array(
            'flightInfoFlightsNotificationsAdmin::admin_form'
        ), 
        'access arguments' => array(
            'access administration pages'
        ), 
        'type' => MENU_NORMAL_ITEM
    );
    
    return $items;
  }
  
  function admin_form($form, &$form_state) {
    $form = array();
    
    /*
     * Test Mode Switch
     * Disable SMTP & SMS
     */

    $form['flighinfo_notifications_test_mode'] = array(
      '#type' => 'select', 
      '#title' => t('Test Mode'), 
      '#options' => array(
          'devel' => t('Developer Only'), 
          'admin_only' => ('Admins Only'),
          'off' => t('Off'), 
      ), 
      '#default_value' => variable_get('flighinfo_notifications_test_mode', 'on'), 
      '#description' => t("Completely disable SMS & email messages for testing or restrict sending to admins. <span style='color:darkred;'>There are safety toggles for SMTP at <a href='/admin/config/system/smtp'>admin/config/system/smtp</a> and for SMS at <a href='admin/config/flightinfo/sms'>admin/config/flightinfo/sms</a> as well. All must be enabled for production messages<span>"),
    );    
    
    
    /*
     * TODO
     * Make the number of emails adjustable, but okay for now
     */
    // $admin_notification_emails = variable_get('admin_notification_emails', array());
    $admin_notification_emails_phonenumbers = variable_get('admin_notification_emails_phonenumbers', array());
    $total = 5;
  
    $form['admin_notification_emails_phonenumbers'] = array(
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#title' => t('Flight Delay Notification Admin Emails & Phone Numbers'),
    );
  
    for ($i=1; $i<$total+1; $i++) {
      $form['admin_notification_emails_phonenumbers']['email_'. $i] = array(
        '#type' => 'emailfield',
        '#title' => t('Email ' . $i), // %i', array('%i' => $i)),
        '#default_value' => isset($admin_notification_emails_phonenumbers['email_'. $i]) ? $admin_notification_emails_phonenumbers['email_'. $i] : '',
      );
      
      $form['admin_notification_emails_phonenumbers']['phone_'. $i] = array(
        '#type' => 'textfield',
        '#title' => t('Phone ' . $i), // %i', array('%i' => $i)),
        '#default_value' => isset($admin_notification_emails_phonenumbers['phone_'. $i]) ? $admin_notification_emails_phonenumbers['phone_'. $i] : '',
        '#description' => t('format as +18803301234 no dashes or spaces & will not work without leading +1 country code'),
      );
      
    }
    
    $devel_notification_emails_phonenumbers = variable_get('devel_notification_emails_phonenumbers', array());
    $total = 3;
    $form['devel_notification_emails_phonenumbers'] = array(
      '#type' => 'fieldset',
      '#tree' => TRUE,
      '#title' => t('SOFTWARE DEVELOPER ONLY DEBUGGING TOOLS'),
    );
  
    for ($i=1; $i<$total+1; $i++) {
      $form['devel_notification_emails_phonenumbers']['email_'. $i] = array(
        '#type' => 'emailfield',
        '#title' => t('Email ' . $i), // %i', array('%i' => $i)),
        '#default_value' => isset($devel_notification_emails_phonenumbers['email_'. $i]) ? $devel_notification_emails_phonenumbers['email_'. $i] : '',
      );
      
      $form['devel_notification_emails_phonenumbers']['phone_'. $i] = array(
        '#type' => 'textfield',
        '#title' => t('Phone ' . $i), // %i', array('%i' => $i)),
        '#default_value' => isset($devel_notification_emails_phonenumbers['phone_'. $i]) ? $devel_notification_emails_phonenumbers['phone_'. $i] : '',
        '#description' => t('format as +18803301234 no dashes or spaces & will not work without leading +1 country code'),
      );
      
    }
    
    
    $form['server_info'] = array(
      '#markup' => 'Flightinfo Notifications Admin'
    );
    
    
    return system_settings_form($form);
}
  
  
}