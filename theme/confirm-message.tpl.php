<?php
/**
 * @file msnf-form-step-info.tpl.php
 *
 * Displays information about the current form step.
 *
 * Available variables:
 * 
 */
?>
<div class="confirmation-text"> 
  <div class="email-confirm"> 
    <h4>Email Message:</h4> 
    <div>
      <p><b>SUBJECT: </b><br><?php print $subject; ?></p>
    </div>
      <div><p><b>MESSAGE: </b><br>
      <?php print $message; ?></p>
    </div>
  </div>
  
  <div class="sms-confirm">
    <h4>SMS Text Message:</h4> 
    <div>
      <p><b>SMS SUBJECT: </b><br><?php print $sms_subject; ?></p>
    </div>
      <div><p><b>SMS MESSAGE: </b><br>
      <?php print $sms_message; ?></p>
    </div>
  </div>
</div>    
  