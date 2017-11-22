<?php
/**
 * Created by PhpStorm.
 * User: admin
 * Date: 05-Jan-2017
 * Time: 7:51 PM
 */

namespace Drupal\shm_mmr\Form;
use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\shm_mmr\MMRReport;

class GenerateReport extends FormBase{
  public function getFormId() {
    return 'mmr_generate_report';
    // TODO: Implement getFormId() method.
  }

  public function buildForm(array $form, FormStateInterface $form_state) {
    // TODO: Implement buildForm() method.
    $form['month'] = array(
      '#type' => 'select',
      '#title' => 'Month',
      '#default_value' => date('m'),
      '#required' => TRUE,
      '#options' => array(
        1 => 'january',
        2 => 'February',
        3 => 'March',
        4 => 'April',
        5 => 'May',
        6 => 'June',
        7 => 'July',
        8 => 'August',
        9 => 'September',
        10 => 'October',
        11 => 'November',
        12 => 'December',
      ),
    );

    $years = array();
    for ($i = 2014; $i <= date('Y'); $i++){
      $years[$i] = $i;
    }

    $form['year'] = array(
      '#type' => 'select',
      '#title' => 'Year',
      '#required' => TRUE,
      '#default_value' => date('Y'),
      '#options' => $years,
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Generate',
    );

    return $form;
  }

  public function submitForm(array &$form, FormStateInterface $form_state) {
    // TODO: Implement submitForm() method.
    $reportGenerator = new MMRReport($form_state->getValue('month'), $form_state->getValue('year'));
    $reportGenerator->generate();
  }
}