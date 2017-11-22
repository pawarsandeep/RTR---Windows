<?php
/**
 * Created by PhpStorm.
 * User: Piyusha Pokharana
 * Date: 11/16/2017
 * Time: 2:24 PM
 */

namespace Drupal\shm_ajs\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\node\Entity\Node;
use Drupal\shm_services_entity\Entity\JobSubmission;

/**
 * Class DeleteJobForm
 * @package Drupal\shm_ajs
 */
class DeleteJobForm extends FormBase{



    public function getFormId() {
        return 'delete_job_form';
    }
    /**
     * @param \Drupal\shm_services_entity\Entity\JobSubmission $job_entity
     * @return array
     */
    public function buildForm(array $form, FormStateInterface $form_state,JobSubmission $job_entity = null) {
        //$job_entity = $job_entity->id();
        $form['confirm-message'] = array(
            '#markup' => t('This action cannot be undone.'),
            '#prefix' => '<div class="confirm-message">',
            '#suffix' => '</div>',

        );
        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Delete'),
            '#attributes' => array('class' => array('btn-danger', 'icon-before','button--primary')),
        );
        $form['job_id'] = $job_entity->id();

        $form['confirm-cancel'] = array(
            '#markup' => 'Cancel',
            '#prefix' => '<a class="button" data-drupal-selector="edit-cancel" id="edit-cancel" href="/service/'.$job_entity->id().'">',
            '#suffix' => '</a>',
        );


        return $form;
    }

    public function submitForm(array &$form, FormStateInterface $form_state) {
        $job_entity = $form['job_id'];


        if($_SERVER['REDIRECT_URL'] =='/liferaft/'.$job_entity.'/delete')
        {
            $reinspec_info = JobSubmission::load($job_entity);
            $gas_cylin_id = $reinspec_info->get('field_liferaft_agas_cylinder')->getString();
            $lifed_id = $reinspec_info->get('field_liferaft_lifed_equipments')->getString();

            $gas_cylin_ids = explode(", ", $gas_cylin_id);
            $lifed_ids = explode(", ", $lifed_id);


            if ($gas_cylin_ids) {
                foreach ($gas_cylin_ids as $entity) {

                    $gas_cylinder_info = JobSubmission::load($entity);
                    $gas_cylinder_info->delete();
                }
            }
            if ($lifed_ids) {
                foreach ($lifed_ids as $entity) {
                    $lifed_info = JobSubmission::load($entity);
                    $lifed_info->delete();
                }
            }

            $reinspec_info->delete();
        }
        if($_SERVER['REDIRECT_URL'] =='/condemnation/'.$job_entity.'/delete')
        {
            $condemnation_info = JobSubmission::load($job_entity);
            drupal_set_message("hello");
            $gas_cylin_id = $condemnation_info->get('field_cylinders_details')->getString();

            $gas_cylin_ids = explode(", ", $gas_cylin_id);

            if ($gas_cylin_ids) {
                foreach ($gas_cylin_ids as $entity) {

                    $gas_cylinder_info = JobSubmission::load($entity);
                    $gas_cylinder_info->delete();
                }
            }

            $condemnation_info->delete();
        }
       //$form_state->setRedirectUrl('/node/691');
        drupal_set_message("The job submission has been deleted.");
        $form_state->setRedirect('<front>');
    }
}