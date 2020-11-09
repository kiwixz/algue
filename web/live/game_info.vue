<template>
  <div class="game_info">
    <div>game type</div>
    <div>bans</div>
  </div>
</template>

<style lang="scss">
  .game_info {
    display: flex;
    flex-wrap: wrap;
    justify-content: space-between;
  }
</style>

<script lang="ts">
  import Vue from "vue";
  import Component from "vue-class-component";
  import { Prop } from "vue-property-decorator";

  @Component
  export default class extends Vue {
    @Prop() readonly game: object | undefined;
  }
</script>
