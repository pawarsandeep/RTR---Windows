<?php
/**
 * Created by PhpStorm.
 * User: Sandeep
 * Date: 09-Jan-2017
 * Time: 7:25 PM
 */

namespace Drupal\shm_mmr;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Url;
use Drupal\eform\Entity\EFormSubmission;
use Drupal\Core\Mail\MailManagerInterface;
use Symfony\Component\HttpFoundation\RedirectResponse;

class MmrController extends ControllerBase{
  public function sendReport(EFormSubmission $eform_submission) {
    $module = 'shm_mmr';
    $key = 'send_mmr';
    $to = $this->getRecipient($eform_submission);//'sandeep.pawar@fantailtech.com';
    $lang = \Drupal::languageManager()->getDefaultLanguage();
    $params = $this->getMailParams($eform_submission);
    $from = $this->getFrom($eform_submission);
    $send = TRUE;

    $mailManager = \Drupal::service('plugin.manager.mail');
    $res = $mailManager->mail($module, $key, $to, $lang, $params, $from, $send);
    if ($res['result']) {
      $eform_submission->set('field_mail_sent', 1);
      $eform_submission->save();
      drupal_set_message("The report has been sent successfully..!", 'status');
    }
    else{
      drupal_set_message("Oops..! Failed to send the report.", 'error');
    }
    $url = Url::fromRoute('entity.eform_submission.canonical', ['eform_submission' => $eform_submission->id()]);
    return(new RedirectResponse($url->toString()));
  }

  /* public function updateEntityDepartment(){

       // $a = 10;
      $job_query = db_select('eform_submission', 'e_sub')
     ->fields('e_sub', array('eid'))
      ->condition('type','invoice','=')
     ->execute()
      ->fetchAll();

      $b = \Drupal::entityTypeManager()->getStorage('eform_submission');
      foreach ($job_query as $key=>$job) {
          $job_id = $b->load($job->eid);
          $job_id->set('field_department','496');
          $job_id->save();
         
      }
	   drupal_set_message("updated eform...".count($job_query));
        return(array('#markup' => '<h1>Hello</h1>'));
    }*/
	
  protected function getMailParams(EFormSubmission $manufReport){
    $param = array();
    $mailAttachments = array();
    $param['body'] = $manufReport->get('field_email_body')->value;
    $param['subject'] = 'SHM Lifeboat OEM Report - ' . date('M-Y', strtotime($manufReport->get('field_report_month')->getString()));
    $attachments = $manufReport->get('field_mail_attachments')->referencedEntities();
    foreach ($attachments as $attachment){
      $file = new \stdClass();
      $file->uri = $attachment->getFileUri();
      $file->filename = $attachment->getFileName();
      $file->filemime = $attachment->getMimeType();
      $mailAttachments[] = $file;
    }
    $param['files'] = $mailAttachments;
    $param['headers'] = array(
      'Bcc' => 'archive@fantailtech.com',
      'Cc' => $this->getCc($manufReport),
      'From' => $this->getFrom($manufReport),
    );
    return($param);
  }

  protected function getRecipient(EFormSubmission &$manufReport){
    $recipient = NULL;
    $contacts = $manufReport->get('field_manuf_report_send_to')->referencedEntities();
    foreach ($contacts as $key => $contact){
      if ($key > 0){
        $recipient .= ',' . $contact->getEmail();
      }
      else {
        $recipient .= $contact->getEmail();
      }
    }
    return($recipient);
  }

  protected function getFrom(EFormSubmission $manufReport){
    $from = $manufReport->get('field_mmr_from')->getString();
    return($from);
  }

  protected function getCc(EFormSubmission $manufReport){
    $cc = $manufReport->get('field_mmr_cc')->getString();
    return($cc);
  }
}