<?php
namespace Drupal\shm_d3_visualization\Plugin\Block;
use Drupal\Core\Block\BlockBase;
use Drupal\Core\Form\FormBuilderInterface;
use Drupal\Core\Form\FormStateInterface;
/**
 * Provides block HTML for D3 to draw Pie Chart to display Jobs
 * on Admin Dashboard.
 *
 * @Block(
 *   id = "bar_chart_jobs_by_country_admin",
 *   admin_label = @Translation("Bar Chart Jobs by Country Admin"),
 *   category = "Dashboard Visualization"
 * )
 */

class BarChartJobsByCountryAdmin extends  BlockBase{
  public function build() {
    return array(
        /*'#type' => 'inline_template',
        '#template' => '<div class = "admin dashboard jobs pie-chart">
                    <svg id = "pie-chart-jobs-admin"></svg></div>',
        '#context' => array(),
        */
        '#markup' => '<div class="panel panel-info">
							<div class="panel-heading">
								<h3 class="panel-title">Jobs by Country</h3>
							</div>
							<div class="panel-body">
								<div id = "bar-chart-jobs-by-country-admin" style = "position:relative;">
								<div id="tooltip-admin-jobs-by-country-bar-chart" class="chart tool-tip hidden">
								<p><strong>Important Label Heading</strong></p>
								<p><span id="value">100</span>%</p>
							</div>
							</div>
						</div>',
        '#attached' => array(
            'library' => array(
                'shm_d3_visualization/bar_chart_jobs_by_country_admin',
                'shm_d3_visualization/shm_d3_visualization'
            )
        )
    );
  }

  /**
   * {@inheritdoc}
   */
  public function blockForm($form, FormStateInterface $form_state) {
    $form = parent::blockForm($form, $form_state);
    return $form;
  }
}