<?php
namespace Drupal\shm_d3_visualization\Plugin\Block;
use Drupal\Core\Block\BlockBase;
use Drupal\Core\Form\FormBuilderInterface;
use Drupal\Core\Form\FormStateInterface;
/**
 * Provides block HTML for D3 to draw Stacked Bar Chart to display Invoices
 * on Admin Dashboard.
 *
 * @Block(
 *   id = "bar_chart_royalty_due_by_month_admin",
 *   admin_label = @Translation("Bar Chart Royalty Due by Month Admin"),
 *   category = "Dashboard Visualization"
 * )
 */

class RoyaltyDueByMonth extends  BlockBase{
  public function build() {
    return array(
        /*'#type' => 'inline_template',
        '#template' => '<div class = "admin dashboard jobs pie-chart">
                    <svg id = "pie-chart-jobs-admin"></svg></div>',
        '#context' => array(),
        */
        '#markup' => '<div class="panel panel-info">
							<div class="panel-heading">
								<h3 class="panel-title">Royalty Due by Month</h3>
							</div>
							<div class="panel-body">
								<div id = "bar-chart-royalty-due-by-month-admin" style = "position:relative;">
								<div id="tooltip-admin-royalty-due-by-month-bar-chart" class="chart bar-tool-tip hidden">
								<strong>Important Label Heading</strong>
								<span id="value">100</span>
							</div>
							</div>
						</div>',
        '#attached' => array(
            'library' => array(
                'shm_d3_visualization/bar_chart_royalty_due_by_month_admin',
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