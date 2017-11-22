<p>Enter your CEIR/IAEE username and password below.</p>

<div class="member-login-form-wrapper">
  <?php
	print drupal_render($form['name']);
	print drupal_render($form['pass']);
   ?>
   <div class="forgot-cv-credentials">
   	 <a href="https://members.iaee.com/cvweb/cgi-bin/utilities.dll/openpage?wrp=forgotPasswordForm.htm">Forgot username or password?</a>
   </div>
   <div class="cv-register">
     <span>Not a member? Register </span><a href="https://members.iaee.com/cvweb/cgi-bin/memberdll.dll/info?wrp=customer_new.htm">here</a><span>.</span>
   </div>


	<?php
	print drupal_render($form['form_build_id']);
	print drupal_render($form['form_id']);
	print drupal_render($form['actions']);
	?>

</div>