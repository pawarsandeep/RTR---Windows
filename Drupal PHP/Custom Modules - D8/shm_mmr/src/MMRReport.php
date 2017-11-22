<?php
namespace Drupal\shm_mmr;

use Drupal\Core\Controller\ControllerBase;
use \Drupal\Core\Database\Database;
use \Drupal\node\Entity\Node;
use \Drupal\Core\Asset\AttachedAssets;
use \Drupal\user\Entity\User;
use \Drupal\eform\Entity\EFormSubmission;
class MMRReport extends ControllerBase {
  protected $report;
  protected $directory;
  protected $connection;
  protected $monthSelection;
  protected $yearSelection;
  protected $pdfCover;

  function __construct($monthSelection, $yearSelection) {
    $this->monthSelection = $monthSelection;
    $this->yearSelection = $yearSelection;
    $this->directory = $this->getDir();
    $this->connection = Database::getConnection();
    $this->pdfCover = array(
      '#type' => 'markup',
      '#markup' => \Drupal::config('shm_mmr.settings')->get('pdf.cover.value'),
  );
  }

  /**
   * @param $attachedAssets
   * @return array
   */
  public function generate(){
    $manuf_nids = $this->getManufNids();
    $report = NULL;
		//Loop through each manufacturer.
    foreach ($manuf_nids as $test=>$manuf_nid){
      $matched_jobs = $this->getMatchedJobs($manuf_nid);
      if (count($matched_jobs) == 0){
        continue;
      }
      $mmrFileName = $this->getMmrFileName($manuf_nid->nid);
      //Loop through each matched job.
      foreach ($matched_jobs as $key=>$job) {
        if ($key == 0) {
          $attachments = NULL;
          $report = NULL;
        }
        $lineItems = NULL;
        $companyName = NULL;
        $b = \Drupal::entityTypeManager()->getStorage('eform_submission');
        $job = $b->load($job->eid);
        $jobId = $job->get('field_job_id')->getString();
		
      /*  foreach ($job->get('field_boat_checklist')->getValue() as $boat_check){
          $attachments[] = $boat_check;
        }
        foreach ($job->get('field_davit_checklist')->getValue() as $davit_check){
          $attachments[] = $davit_check;
        }*/
        if($job->get('field_company_name')->first()) {
          $companyName = $job->get('field_company_name')
            ->first()
            ->get('entity')
            ->getTarget()
            ->get('title')
            ->getString();
        }
			$davit_flag = false;
			$boat_flag = false;
        //Loop through each line item in the job.
        foreach ($job->get('field_certificate_line_item') as $lineItemReference){
          $lineItem = $lineItemReference->get('entity')->getTarget();
		  $davit_id = $lineItem->get('field_davit_oem_name')->getString();
		  $boat_id = $lineItem->get('field_boat_oem_name')->getString();
		  
		  $davit_node = \Drupal\node\Entity\Node::load($davit_id);
		  $boat_node = \Drupal\node\Entity\Node::load($boat_id);
			
         if(!empty($boat_id) and empty($davit_id))
			{
				$lineItems[] = array(
            'davit_sr_no' => $lineItem->get('field_davit_sr_no')->getString(),
            'boat_sr_no' => $lineItem->get('field_boat_sr_no')->getString(),
            'station' => $lineItem->get('field_lb_station')->getString(),
            'davit_type' => $lineItem->get('field_davit_type')->getString(),
            'boat_type' => $lineItem->get('field_boat_type')->getString(),
            'winch_type' => $lineItem->get('field_winch_type')->getString(),
			'boat_oem_name' => $boat_node->getTitle(),
          );
			}
			elseif(!empty($davit_id) and empty($boat_id)){
				$lineItems[] = array(
            'davit_sr_no' => $lineItem->get('field_davit_sr_no')->getString(),
            'boat_sr_no' => $lineItem->get('field_boat_sr_no')->getString(),
            'station' => $lineItem->get('field_lb_station')->getString(),
            'davit_type' => $lineItem->get('field_davit_type')->getString(),
            'boat_type' => $lineItem->get('field_boat_type')->getString(),
            'winch_type' => $lineItem->get('field_winch_type')->getString(),
			'davit_oem_name' => $davit_node->getTitle(),
          );
			}
			elseif(!empty($davit_id) and !empty($boat_id)){
			$lineItems[] = array(
            'davit_sr_no' => $lineItem->get('field_davit_sr_no')->getString(),
            'boat_sr_no' => $lineItem->get('field_boat_sr_no')->getString(),
            'station' => $lineItem->get('field_lb_station')->getString(),
            'davit_type' => $lineItem->get('field_davit_type')->getString(),
            'boat_type' => $lineItem->get('field_boat_type')->getString(),
            'winch_type' => $lineItem->get('field_winch_type')->getString(),
			'davit_oem_name' => $davit_node->getTitle(),
			'boat_oem_name' => $boat_node->getTitle(),
          );
			}
          /*----------------------Checklist as per boat and davit----------*/
			$man_id = $manuf_nid->nid;
			if($man_id == $davit_id && $man_id == $boat_id)
			{
					$davit_flag = true;
					$boat_flag = true;
			}
			elseif($man_id == $boat_id)
			{
				$boat_flag = true;
			}
			elseif($man_id == $davit_id)
			{
				$davit_flag = true;
			}
		}
		if($davit_flag == true && $boat_flag == true)
		 {
				foreach ($job->get('field_davit_checklist')->getValue() as $davit_check){
						$attachments[$davit_check['target_id']] = $davit_check;
					}
		  
				foreach ($job->get('field_boat_checklist')->getValue() as $boat_check){
						$attachments[$boat_check['target_id']] = $boat_check;
					}	
		 }
		 elseif($davit_flag == true)
		 {
			 foreach ($job->get('field_davit_checklist')->getValue() as $davit_check){
						$attachments[$davit_check['target_id']] = $davit_check;
					}
		 }
		 elseif($boat_flag == true)
		 {
			 foreach ($job->get('field_boat_checklist')->getValue() as $boat_check){
						$attachments[$boat_check['target_id']] = $boat_check;
					}
		 }

        $report[$jobId] = array(
          'job_id' => $jobId,
          'vessel_name' => $job->get('field_vessel_name')->getString(),
          'company_name' => $companyName,
          'line_items' => $lineItems,
        );
        //$oem = $lineitem->get('field_davit_oem_name')->first();
        //$abc = $oem->get('entity')->getTarget();

      }

      if (count($matched_jobs) != 0){
        $month = date('Y-m-01', strtotime($this->yearSelection . '-' . $this->monthSelection));
        $contactName = $this->getContactName($manuf_nid->nid);
        $sendTo = $this->getSendTo($manuf_nid->nid);
        $reportId = $this->getReportId();
        // Prepare HTML for PDF generation.
        $renderArray = array('#theme' => 'shm_mmr_pdf',
          '#attached' => array('library'=>array('shm_mmr/shm_mmr')),
          '#cover' => $this->pdfCover,
          '#report' => $report,
          '#report_id' => $reportId,
          '#month_year' => date('M-Y', strtotime($this->yearSelection . '-' . $this->monthSelection)),
          '#manufacturer' => $this->getManufName($manuf_nid->nid));
        $assetResolver = \Drupal::service('asset.resolver');
        $cssCollectionRenderer = \Drupal::service('asset.css.collection_renderer');
        $css_assets = $assetResolver->getCssAssets(AttachedAssets::createFromRenderArray($renderArray), TRUE);
        $rendered_css = $cssCollectionRenderer->render($css_assets);
        $renderArray['#mmr_css'] = \Drupal::service('renderer')->render($rendered_css);
        $html = (string) \Drupal::service('renderer')->render($renderArray);
        $pdfGenerator = new PDFGenerator();
        $mmrFile = $pdfGenerator->generatePdf($html, $this->directory, $mmrFileName);
        $attachments[] = array(
          'target_id' => $mmrFile->id(),
          'display' => 1,
          'description' =>'',
        );
        // Create EFormSubmission.
        $entity = EFormSubmission::create( array(
          'type' => 'lifeboat_monthly_manufacturer_re',
        ));

        // Initialize EFormSubmission field values.
        $body = NULL;
        $config = \Drupal::service('config.factory')->get('shm_mmr.settings');
        $body = $config->get('email.body');
        $body = str_replace('[name]', $contactName, $body);
        $body = str_replace('[batch]', date('F-Y', strtotime($this->yearSelection . '-' . $this->monthSelection)), $body);


        //Assign field values to newly created EFormSubmission.
        $entity->field_email_body = $body;
        $entity->field_mail_sent = 0;
        $entity->field_manufacturer = $manuf_nid->nid;
        $entity->field_manuf_report_send_to = $sendTo;
        $entity->field_mmr_cc = $config->get('email.cc');
        $entity->field_report_id = $reportId;
        $entity->field_report_month = $month;
        $entity->field_mail_attachments = $attachments;
        $entity->save();

      }
    }
    drupal_set_message("The reports generated successfully for the month $this->monthSelection $this->yearSelection.", 'status');
    return (array('#theme' => 'shm_mmr_pdf', '#report' => $report));
  }

  /**
   * @return string
   */
  protected function getDir(){
    $monthYear = trim($this->monthSelection . $this->yearSelection, ' ');
    return("private://mmr/$monthYear" . "/");
  }

  /**
   * @return mixed
   */
  protected function getManufNids() {
    $manuf_query = $this->connection->select('node', 'n');
    $manuf_query->fields('n', ['nid']);
    $manuf_query->innerJoin('node__field_comp', 'comp_type', 'comp_type.entity_id = n.nid', []);
    $manuf_query->condition('type', 'company', '=');
    $manuf_query->condition('comp_type.field_comp_target_id', '46', '=');
    $result = $manuf_query->execute();
    return($result->fetchAll());
  }

  /**
   * @param $manuf_nid
   * @return mixed
   */
  protected function getMatchedJobs($manuf_nid){
    $job_query = $this->connection->select('eform_submission', 'e_sub');
    $job_query->fields('e_sub', ['eid']);
    $job_query->leftJoin('eform_submission__field_certificate_line_item', 'cert_lineitem', 'cert_lineitem.entity_id = e_sub.eid');
    $job_query->leftJoin('eform_submission__field_davit_oem_name', 'davit_oem', 'davit_oem.entity_id = cert_lineitem.field_certificate_line_item_target_id');
    $job_query->leftJoin('eform_submission__field_boat_oem_name', 'boat_oem', 'boat_oem.entity_id = cert_lineitem.field_certificate_line_item_target_id');
    $job_query->leftJoin('eform_submission_field_data', 'field_data', 'field_data.eid = e_sub.eid');
    $job_query->leftJoin('eform_submission__field_date_of_inspection', 'doi', 'doi.entity_id = e_sub.eid');
    $andCondition_1 = $job_query->orConditionGroup()
      ->condition('davit_oem.field_davit_oem_name_target_id', $manuf_nid->nid, '=')
      ->condition('boat_oem.field_boat_oem_name_target_id', $manuf_nid->nid, '=');
    $job_query->condition($andCondition_1);
    $job_query->condition('e_sub.type', 'certificate', '=');
    $job_query->condition('field_data.moderation_state', 'approved', '=');
    //$job_query->condition('doi.field_date_of_inspection_value', ['2016-11-01', '2016-11-31'], 'between');
    $job_query->where('MONTH(doi.field_date_of_inspection_value) = ' . $this->monthSelection);
    $job_query->where('YEAR(doi.field_date_of_inspection_value) = ' . $this->yearSelection);
    $result = $job_query->execute();
    return($result->fetchAll());
  }
  /**
   * Returns contact persons uid for current manufacturer.
   *
   * @param int $manuf_nid nid of current manufacturer in loop.
   *
   * @return array $sendTo Array of manufacturers contact/s.
   */
  protected function getSendTo($nid){
    $manuf_contact_query = $this->connection->select('users', 'users');
    $manuf_contact_query->fields('users', ['uid']);
    $manuf_contact_query->leftJoin('user__field_company', 'company', 'company.entity_id = users.uid');
    $manuf_contact_query->condition('company.field_company_target_id', $nid, '=');
    $manuf_contact_result = $manuf_contact_query->execute();
    $manuf_contact_uids = $manuf_contact_result->fetchAll();
    $sendTo = NULL;
    //A manufacturer may have more than one contacts.
    foreach ($manuf_contact_uids as $manuf_contact_uid){
      $sendTo[] = $manuf_contact_uid->uid;
    }
    return($sendTo);
  }

  /**
   * @param $manuf_nid
   * @return string
   */
  protected function getMmrFileName($manuf_nid) {
    $manufacturer = Node::load($manuf_nid);
    $manufName = substr($manufacturer->getTitle(), 0, 8);
    $manufName = str_replace(['\\', '/', ' '], ['-' ,'-', '-'], $manufName);
     return(trim($this->monthSelection . '-' . $this->yearSelection, ' ') . '-' . $manuf_nid . '-' . $manufName);
  }

  /**
   * @return mixed
   */
  protected function getReportId(){
    $datePart = date('ym', strtotime($this->yearSelection . '-' . $this->monthSelection));
    $report_id_inner_query = $this->connection->select('eform_submission__field_report_id', 'report_id_table');
    $report_id_inner_query->fields('report_id_table',['field_report_id_value']);
    $report_id_inner_query->condition('report_id_table.bundle', 'lifeboat_monthly_manufacturer_re', '=');
    $report_id_inner_query->condition('report_id_table.deleted', 0, '=');
    $report_id_inner_query->where("SUBSTR(report_id_table.field_report_id_value, 1, 4) = $datePart");
    $report_id_query = $this->connection->select($report_id_inner_query, 'report_id_table');
    $report_id_query->addExpression('max(CAST(report_id_table.field_report_id_value as UNSIGNED))', 'last_report_id');
    $report_id_result = $report_id_query->execute();
    $max_report_id = $report_id_result->fetchField();
    if (!empty($max_report_id)) {
      $nextReportId = ++$max_report_id;
    }
    else{
      $nextReportId = $datePart . '001';
    }
    return($nextReportId);
  }

  protected function getManufName($manufId){
    $manuf = Node::load($manufId);
    return($manuf->getTitle());
  }

  protected function getContactName($manufId){
    $manuf_contact_query = $this->connection->select('users', 'users');
    $manuf_contact_query->fields('users', ['uid']);
    $manuf_contact_query->leftJoin('user__field_company', 'company', 'company.entity_id = users.uid');
    $manuf_contact_query->condition('company.field_company_target_id', $manufId, '=');
    $manuf_contact_result = $manuf_contact_query->execute();
    $manuf_contact_uids = $manuf_contact_result->fetchAll();
    $manufName = NULL;
    //A manufacturer may have more than one contacts.
    foreach ($manuf_contact_uids as $key => $manuf_contact_uid){
      $user = User::load($manuf_contact_uid->uid);
      if ($key > 0){
        $manufName .= ' ,' . trim($user->get('field_first_name')->getString(), " ");
      }
      else{
        $manufName = trim($user->get('field_first_name')->getString(), " ");
      }

    }
    return($manufName == NULL ? 'Sir/Madam' : $manufName );
  }
}